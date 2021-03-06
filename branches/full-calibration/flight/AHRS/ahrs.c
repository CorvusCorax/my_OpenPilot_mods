/**
 ******************************************************************************
 * @addtogroup AHRS AHRS
 * @brief The AHRS Modules perform
 *
 * @{
 * @addtogroup AHRS_Main
 * @brief Main function which does the hardware dependent stuff
 * @{
 *
 *
 * @file       ahrs.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      INSGPS Test Program
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* OpenPilot Includes */
#include "ahrs.h"
#include "ahrs_adc.h"
#include "ahrs_timer.h"
#include "ahrs_spi_comm.h"
#include "insgps.h"
#include "CoordinateConversions.h"

#define MAX_OVERSAMPLING 50    /* cannot have more than 50 samples      */
#define INSGPS_GPS_TIMEOUT 2   /* 2 seconds triggers reinit of position */
#define INSGPS_GPS_MINSAT  7   /* 2 seconds triggers reinit of position */
#define INSGPS_GPS_MINPDOP 3.5 /* minimum PDOP for postition updates    */
#define INSGPS_MAGLEN       1000
#define INSGPS_MAGTOL      0.2 /* error in magnetic vector length to use  */

// For debugging the raw sensors
//#define DUMP_RAW
//#define DUMP_FRIENDLY
//#define DUMP_EKF

#ifdef DUMP_EKF
#define NUMX 13			// number of states, X is the state vector
#define NUMW 9			// number of plant noise inputs, w is disturbance noise vector
#define NUMV 10			// number of measurements, v is the measurement noise vector
#define NUMU 6			// number of deterministic inputs, U is the input vector
extern float F[NUMX][NUMX], G[NUMX][NUMW], H[NUMV][NUMX];	// linearized system matrices
extern float P[NUMX][NUMX], X[NUMX];	// covariance matrix and state vector
extern float Q[NUMW], R[NUMV];	// input noise and measurement noise variances
extern float K[NUMX][NUMV];	// feedback gain matrix
#endif

volatile int8_t ahrs_algorithm;

/**
 * @addtogroup AHRS_Structures Local Structres
 * @{
 */

//! Contains the data from the mag sensor chip
struct mag_sensor {
	uint8_t id[4];
	uint8_t updated;
	struct {
		int16_t axis[3];
	} raw;
	struct {
		float axis[3];
	} scaled;
	struct {
		float bias[3];
		float scale[3];
		float variance[3];
	} calibration;
} mag_data;

//! Contains the data from the accelerometer
struct accel_sensor {
	struct {
		uint16_t x;
		uint16_t y;
		uint16_t z;
	} raw;
	struct {
		float x;
		float y;
		float z;
	} filtered;
	struct {
		float scale[3][4];
		float variance[3];
	} calibration;
} accel_data;

//! Contains the data from the gyro
struct gyro_sensor {
	struct {
		uint16_t x;
		uint16_t y;
		uint16_t z;
	} raw;
	struct {
		float x;
		float y;
		float z;
	} filtered;
	struct {
		float bias[3];
		float scale[3];
		float variance[3];
	} calibration;
	struct {
		uint16_t xy;
		uint16_t z;
	} temp;
} gyro_data;

//! Conains the current estimate of the attitude
struct attitude_solution {
	struct {
		float q1;
		float q2;
		float q3;
		float q4;
	} quaternion;
} attitude_data;

//! Contains data from the altitude sensor
struct altitude_sensor {
	float altitude;
	bool updated;
} altitude_data;

//! Contains data from the GPS (via the SPI link)
struct gps_sensor {
	float NED[3];
	float heading;
	float groundspeed;
	float quality;
	bool updated;
} gps_data;

/**
 * @}
 */

/* Function Prototypes */
void downsample_data(void);
void calibrate_sensors(void);
void reset_values();
void send_calibration(void);
void send_attitude(void);
void send_velocity(void);
void send_position(void);
void homelocation_callback(AhrsObjHandle obj);
void altitude_callback(AhrsObjHandle obj);
void calibration_callback(AhrsObjHandle obj);
void gps_callback(AhrsObjHandle obj);
void settings_callback(AhrsObjHandle obj);

volatile uint32_t last_counter_idle_start = 0;
volatile uint32_t last_counter_idle_end = 0;
volatile uint32_t idle_counts;
volatile uint32_t running_counts;
uint32_t counter_val;

/**
 * @addtogroup AHRS_Global_Data AHRS Global Data
 * @{
 * Public data.  Used by both EKF and the sender
 */
//! Filter coefficients used in decimation.  Limited order so filter can't run between samples
int16_t fir_coeffs[MAX_OVERSAMPLING];


//! The oversampling rate, ekf is 2k / this
static uint8_t adc_oversampling = 20;


/**
 * @}
 */

/**
 * @brief AHRS Main function
 */
int main()
{
	float gyro[3], accel[3];
	float vel[3] = { 0, 0, 0 };
	uint8_t gps_dirty = 1;
	uint8_t indoor_dirty = 1;
	gps_data.quality = -1;
	uint32_t last_gps_time = 0;
	uint32_t last_indoor_time = 0;
	uint32_t up_time_real = 0;
	uint32_t up_time = 0;
	uint32_t last_up_time = 0;

	ahrs_algorithm = AHRSSETTINGS_ALGORITHM_SIMPLE;

	/* Brings up System using CMSIS functions, enables the LEDs. */
	PIOS_SYS_Init();

	/* Delay system */
	PIOS_DELAY_Init();

	/* Communication system */
	PIOS_COM_Init();

	/* ADC system */
	AHRS_ADC_Config(adc_oversampling);

	/* Setup the Accelerometer FS (Full-Scale) GPIO */
	PIOS_GPIO_Enable(0);
	SET_ACCEL_6G;
#if defined(PIOS_INCLUDE_HMC5843) && defined(PIOS_INCLUDE_I2C)
	/* Magnetic sensor system */
	PIOS_I2C_Init();
	PIOS_HMC5843_Init();
	// Get 3 ID bytes
	strcpy((char *)mag_data.id, "ZZZ");
	PIOS_HMC5843_ReadID(mag_data.id);
#endif

	reset_values();
	INSGPSInit();

	ahrs_state = AHRS_IDLE;
	AhrsInitComms();
	ahrs_state = AHRS_IDLE;
	while(!AhrsLinkReady()) {
		AhrsPoll();
		while(ahrs_state != AHRS_DATA_READY) ;
		ahrs_state = AHRS_PROCESSING;
		downsample_data();
		ahrs_state = AHRS_IDLE;
		if((total_conversion_blocks % 10) == 0)
			PIOS_LED_Toggle(LED1);
	}
/* we didn't connect the callbacks before because we have to wait
for all data to be up to date before doing anything*/
	
	AHRSCalibrationConnectCallback(calibration_callback);
	GPSPositionConnectCallback(gps_callback);
	BaroAltitudeConnectCallback(altitude_callback);
	AHRSSettingsConnectCallback(settings_callback);
	HomeLocationConnectCallback(homelocation_callback);

	calibration_callback(AHRSCalibrationHandle()); //force an update


	/* Use simple averaging filter for now */
	for (int i = 0; i < adc_oversampling; i++)
		fir_coeffs[i] = 1;
	fir_coeffs[adc_oversampling] = adc_oversampling;

#ifdef DUMP_RAW
	int previous_conversion;
	while (1) {
		AhrsPoll();
		int result;
		uint8_t framing[16] =
		    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	  15 };
		while (ahrs_state != AHRS_DATA_READY) ;
		ahrs_state = AHRS_PROCESSING;

		if (total_conversion_blocks != previous_conversion + 1)
			PIOS_LED_On(LED1);	// not keeping up
		else
			PIOS_LED_Off(LED1);
		previous_conversion = total_conversion_blocks;

		downsample_data();
		ahrs_state = AHRS_IDLE;;

		// Dump raw buffer
		result = PIOS_COM_SendBuffer(PIOS_COM_AUX, &framing[0], 16);	// framing header
		result += PIOS_COM_SendBuffer(PIOS_COM_AUX, (uint8_t *) & total_conversion_blocks, sizeof(total_conversion_blocks));	// dump block number
		result +=
		    PIOS_COM_SendBuffer(PIOS_COM_AUX,
					(uint8_t *) & valid_data_buffer[0],
					adc_oversampling *
					ADC_CONTINUOUS_CHANNELS *
					sizeof(valid_data_buffer[0]));
		if (result == 0)
			PIOS_LED_Off(LED1);
		else {
			PIOS_LED_On(LED1);
		}
	}
#endif

	timer_start();

	/******************* Main EKF loop ****************************/
	while(1) {
		AhrsPoll();
		AhrsStatusData status;
		AhrsStatusGet(&status);
		status.CPULoad = ((float)running_counts /
						  (float)(idle_counts + running_counts)) * 100;
		status.IdleTimePerCyle = idle_counts / (timer_rate() / 10000);
		status.RunningTimePerCyle = running_counts / (timer_rate() / 10000);
		status.DroppedUpdates = ekf_too_slow;
		up_time = timer_count();
		if(up_time >= last_up_time) // normal condition
			up_time_real += ((up_time - last_up_time) * 1000) / timer_rate();
		else
			up_time_real += ((0xFFFF - last_up_time + up_time) * 1000) / timer_rate(); 
		last_up_time = up_time;
		status.RunningTime = up_time_real;
		AhrsStatusSet(&status);

		// Alive signal
		if ((total_conversion_blocks % 100) == 0)
			PIOS_LED_Toggle(LED1);

#if defined(PIOS_INCLUDE_HMC5843) && defined(PIOS_INCLUDE_I2C)
		// Get magnetic readings
		// For now don't use mags until the magnetic field is set AND until 5 seconds
		// after initialization otherwise it seems to have problems
		// TODO: Follow up this initialization issue
		HomeLocationData home;
		HomeLocationGet(&home);
		if (PIOS_HMC5843_NewDataAvailable() && 
		    (home.Set = HOMELOCATION_SET_TRUE) && 
		    ((home.Be[0] != 0) || (home.Be[1] != 0) || (home.Be[2] != 0)) &&
		    ((float) timer_count() / timer_rate() > 5)) {
			PIOS_HMC5843_ReadMag(mag_data.raw.axis);
			// Swap the axis here to acount for orientation of mag chip (notice 0 and 1 swapped in raw)
			mag_data.scaled.axis[0] = (mag_data.raw.axis[1] * mag_data.calibration.scale[0]) + mag_data.calibration.bias[0];
			mag_data.scaled.axis[1] = (mag_data.raw.axis[0] * mag_data.calibration.scale[1]) + mag_data.calibration.bias[1];
			mag_data.scaled.axis[2] = (mag_data.raw.axis[2] * mag_data.calibration.scale[2]) + mag_data.calibration.bias[2];
			
			// Only use if magnetic length reasonable
			float Blen = sqrt(pow(mag_data.scaled.axis[0],2) + pow(mag_data.scaled.axis[1],2) + pow(mag_data.scaled.axis[2],2));
			if((Blen < INSGPS_MAGLEN * (1 + INSGPS_MAGTOL)) && (Blen > INSGPS_MAGLEN * (1 - INSGPS_MAGTOL)))
				mag_data.updated = 1;
		}

#endif
		// Delay for valid data

		counter_val = timer_count();
		running_counts = counter_val - last_counter_idle_end;
		last_counter_idle_start = counter_val;

		while (ahrs_state != AHRS_DATA_READY) ;

		counter_val = timer_count();
		idle_counts = counter_val - last_counter_idle_start;
		last_counter_idle_end = counter_val;

		ahrs_state = AHRS_PROCESSING;

		downsample_data();

#ifdef DUMP_EKF
		uint8_t framing[16] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
		// Dump raw buffer
		PIOS_COM_SendBuffer(PIOS_COM_AUX, &framing[0], 16);                                                         // framing header (1:16)
		PIOS_COM_SendBuffer(PIOS_COM_AUX, (uint8_t *) & total_conversion_blocks, sizeof(total_conversion_blocks));  // dump block number (17:20)
		
		PIOS_COM_SendBufferNonBlocking(PIOS_COM_AUX, (uint8_t *) & accel_data.filtered.x, 4*3);                     // accel data (21:32)
		PIOS_COM_SendBufferNonBlocking(PIOS_COM_AUX, (uint8_t *) & gyro_data.filtered.x, 4*3);                      // gyro data (33:44)
		
		PIOS_COM_SendBuffer(PIOS_COM_AUX, (uint8_t *) & mag_data.updated, 1);                                       // mag update (45)
		PIOS_COM_SendBuffer(PIOS_COM_AUX, (uint8_t *) & mag_data.scaled.axis, 3*4);                                 // mag data (46:57)
		PIOS_COM_SendBuffer(PIOS_COM_AUX, (uint8_t *) & gps_data, sizeof(gps_data));                                // gps data (58:82)
		
		PIOS_COM_SendBuffer(PIOS_COM_AUX, (uint8_t *) & X, 4 * NUMX);                                               // X (83:134)
		for(uint8_t i = 0; i < NUMX; i++) 
			PIOS_COM_SendBuffer(PIOS_COM_AUX, (uint8_t *) &(P[i][i]), 4);                                       // diag(P) (135:186)
#endif  
		
		/******************** INS ALGORITHM **************************/
		if (ahrs_algorithm != AHRSSETTINGS_ALGORITHM_SIMPLE) {

			// format data for INS algo
			gyro[0] = gyro_data.filtered.x;
			gyro[1] = gyro_data.filtered.y;
			gyro[2] = gyro_data.filtered.z;
			accel[0] = accel_data.filtered.x,
			accel[1] = accel_data.filtered.y,
			accel[2] = accel_data.filtered.z,

			INSStatePrediction(gyro, accel, 1 / (float)EKF_RATE);
			send_attitude();  // get message out quickly
			send_velocity();
			send_position();
			INSCovariancePrediction(1 / (float)EKF_RATE);

			if (gps_data.updated && ahrs_algorithm == AHRSSETTINGS_ALGORITHM_INSGPS_OUTDOOR) {
				uint32_t this_gps_time = timer_count();
				float gps_delay;
				
				// Detect if greater than certain time since last gps update and if so
				// reset EKF to that position since probably drifted too far for safe
				// update
				if (this_gps_time < last_gps_time)
					gps_delay = ((0xFFFF - last_gps_time) - this_gps_time) / timer_rate();
				else 
					gps_delay = (this_gps_time - last_gps_time) / timer_rate();
				last_gps_time = this_gps_time;
				
				gps_dirty = gps_delay > INSGPS_GPS_TIMEOUT;

				// Compute velocity from Heading and groundspeed
				vel[0] = gps_data.groundspeed *
					cos(gps_data.heading * M_PI / 180);
				vel[1] = gps_data.groundspeed *
					sin(gps_data.heading * M_PI / 180);
				vel[2] = 0;
				
				
				INSSetPosVelVar(0.004);
				
				if (mag_data.updated && !gps_dirty) {
					//TOOD: add check for altitude updates
					FullCorrection(mag_data.scaled.axis,
						       gps_data.NED,
						       vel,
						       altitude_data.
						       altitude);
					mag_data.updated = 0;
				} else if (!gps_dirty) {
					GpsBaroCorrection(gps_data.NED,
							  vel,
							  altitude_data.
							  altitude);
				} else { // GPS hasn't updated for a bit
					INSPosVelReset(gps_data.NED,vel);					
				}

				gps_data.updated = false;
			} else if (ahrs_algorithm == AHRSSETTINGS_ALGORITHM_INSGPS_OUTDOOR
				   && mag_data.updated == 1) {
				MagCorrection(mag_data.scaled.axis);	// only trust mags if outdoors
				mag_data.updated = 0;
			} else {
				// Indoors, update with zero position and velocity and high covariance
				AHRSSettingsData settings;
				AHRSSettingsGet(&settings);
				INSSetPosVelVar(settings.IndoorVelocityVariance);
				vel[0] = 0;
				vel[1] = 0;
				vel[2] = 0;
				
				uint32_t this_indoor_time = timer_count();
				float indoor_delay;
				
				// Detect if greater than certain time since last gps update and if so
				// reset EKF to that position since probably drifted too far for safe
				// update
				if (this_indoor_time < last_indoor_time)
					indoor_delay = ((0xFFFF - last_indoor_time) - this_indoor_time) / timer_rate();
				else 
					indoor_delay = (this_indoor_time - last_indoor_time) / timer_rate();
				last_indoor_time = this_indoor_time;
				
				indoor_dirty = indoor_delay > INSGPS_GPS_TIMEOUT;
				
				if(indoor_dirty)
					INSPosVelReset(vel,vel);					

				if((mag_data.updated == 1) && (ahrs_algorithm == AHRSSETTINGS_ALGORITHM_INSGPS_INDOOR)) {
					MagVelBaroCorrection(mag_data.scaled.axis,
							     vel,altitude_data.altitude);  // only trust mags if outdoors
					mag_data.updated = 0;
				} else {
					VelBaroCorrection(vel, altitude_data.altitude);
				}
			}

			attitude_data.quaternion.q1 = Nav.q[0];
			attitude_data.quaternion.q2 = Nav.q[1];
			attitude_data.quaternion.q3 = Nav.q[2];
			attitude_data.quaternion.q4 = Nav.q[3];
		} else if (ahrs_algorithm == AHRSSETTINGS_ALGORITHM_SIMPLE) {
			float q[4];
			float rpy[3];
	    /***************** SIMPLE ATTITUDE FROM NORTH AND ACCEL ************/
			/* Very simple computation of the heading and attitude from accel. */
			rpy[2] =
			    atan2((mag_data.raw.axis[0]),
				  (-1 * mag_data.raw.axis[1])) * 180 /
			    M_PI;
			rpy[1] =
			    atan2(accel_data.filtered.x,
				  accel_data.filtered.z) * 180 / M_PI;
			rpy[0] =
			    atan2(accel_data.filtered.y,
				  accel_data.filtered.z) * 180 / M_PI;

			RPY2Quaternion(rpy, q);
			attitude_data.quaternion.q1 = q[0];
			attitude_data.quaternion.q2 = q[1];
			attitude_data.quaternion.q3 = q[2];
			attitude_data.quaternion.q4 = q[3];
			send_attitude();

		}

		ahrs_state = AHRS_IDLE;

#ifdef DUMP_FRIENDLY
		PIOS_COM_SendFormattedStringNonBlocking(PIOS_COM_AUX, "b: %d\r\n",
							total_conversion_blocks);
		PIOS_COM_SendFormattedStringNonBlocking(PIOS_COM_AUX,"a: %d %d %d\r\n",
							(int16_t) (accel_data.filtered.x * 1000),
							(int16_t) (accel_data.filtered.y * 1000),
							(int16_t) (accel_data.filtered.z * 1000));
		PIOS_COM_SendFormattedStringNonBlocking(PIOS_COM_AUX, "g: %d %d %d\r\n",
							(int16_t) (gyro_data.filtered.x * 1000),
							(int16_t) (gyro_data.filtered.y * 1000),
							(int16_t) (gyro_data.filtered.z * 1000));
		PIOS_COM_SendFormattedStringNonBlocking(PIOS_COM_AUX,"m: %d %d %d\r\n",
							mag_data.raw.axis[0],
							mag_data.raw.axis[1],
							mag_data.raw.axis[2]);
		PIOS_COM_SendFormattedStringNonBlocking(PIOS_COM_AUX,
							"q: %d %d %d %d\r\n",
							(int16_t) (Nav.q[0] * 1000),
							(int16_t) (Nav.q[1] * 1000),
							(int16_t) (Nav.q[2] * 1000),
							(int16_t) (Nav.q[3] * 1000));
#endif
	}

	return 0;
}

/*
 * @brief Perform calibration of a 3-axis field sensor using an affine transformation
 * matrix.
 *
 * Computes result = scale * arg.
 *
 * @param result[out] The three-axis resultant field.
 * @param scale[in] The 4x4 affine transformation matrix.  The fourth row is implicitly
 * 	[0 0 0 1]
 * @param arg[in] The 3-axis input field.  The 'w' component is implicitly 1.
 */
void calibration(float result[3], float scale[3][4], float arg[3])
{
	for (int row = 0; row < 3; ++row) {
		result[row] = 0.0f;
		int col;
		for (col = 0; col < 3; ++col) {
			result[row] += arg[row] * scale[row][col];
		}
		// fourth column: arg has an implicit w value of 1.0f.
		result[row] += scale[row][col];
	}
}

/**
 * @brief Downsample the analog data
 * @return none
 *
 * Tried to make as much of the filtering fixed point when possible.  Need to account
 * for offset for each sample before the multiplication if filter not a boxcar.  Could
 * precompute fixed offset as sum[fir_coeffs[i]] * ACCEL_OFFSET.  Puts data into global
 * data structures @ref accel_data and @ref gyro_data.
 *
 * The accel_data values are converted into a coordinate system where X is forwards along
 * the fuselage, Y is along right the wing, and Z is down.
 */
void downsample_data()
{
	uint16_t i;

	// Get the Y data.  Third byte in.  Convert to m/s
	float accel_filtered[3];
	accel_filtered[1] = 0;
	for (i = 0; i < adc_oversampling; i++)
		accel_filtered[1] += valid_data_buffer[0 + i * PIOS_ADC_NUM_PINS] * fir_coeffs[i];
	accel_filtered[1] /= (float) fir_coeffs[adc_oversampling];
	// Get the X data which projects forward/backwards.  Fifth byte in.  Convert to m/s
	accel_filtered[0] = 0;
	for (i = 0; i < adc_oversampling; i++)
		accel_filtered[0] += valid_data_buffer[2 + i * PIOS_ADC_NUM_PINS] * fir_coeffs[i];
	accel_filtered[0] /= (float) fir_coeffs[adc_oversampling];

	// Get the Z data.  Third byte in.  Convert to m/s
	accel_filtered[2] = 0;
	for (i = 0; i < adc_oversampling; i++)
		accel_filtered[2] += valid_data_buffer[4 + i * PIOS_ADC_NUM_PINS] * fir_coeffs[i];
	accel_filtered[2] /= (float) fir_coeffs[adc_oversampling];

	float accel_scaled[3];
	calibration(accel_scaled, accel_data.calibration.scale, accel_filtered);
	accel_data.filtered.x = accel_scaled[0];
	accel_data.filtered.y = accel_scaled[1];
	accel_data.filtered.z = accel_scaled[2];

	// Get the X gyro data.  Seventh byte in.  Convert to deg/s.
	gyro_data.filtered.x = 0;
	for (i = 0; i < adc_oversampling; i++)
		gyro_data.filtered.x  += valid_data_buffer[1 + i * PIOS_ADC_NUM_PINS] * fir_coeffs[i];
	gyro_data.filtered.x /= fir_coeffs[adc_oversampling];
	gyro_data.filtered.x = (gyro_data.filtered.x * gyro_data.calibration.scale[0]) + gyro_data.calibration.bias[0];

	// Get the Y gyro data.  Second byte in.  Convert to deg/s.
	gyro_data.filtered.y = 0;
	for (i = 0; i < adc_oversampling; i++)
		gyro_data.filtered.y += valid_data_buffer[3 + i * PIOS_ADC_NUM_PINS] * fir_coeffs[i];
	gyro_data.filtered.y /= fir_coeffs[adc_oversampling];
	gyro_data.filtered.y = (gyro_data.filtered.y * gyro_data.calibration.scale[1]) + gyro_data.calibration.bias[1];

	// Get the Z gyro data.  Fifth byte in.  Convert to deg/s.
	gyro_data.filtered.z = 0;
	for (i = 0; i < adc_oversampling; i++)
		gyro_data.filtered.z += valid_data_buffer[5 + i * PIOS_ADC_NUM_PINS] * fir_coeffs[i];
	gyro_data.filtered.z /= fir_coeffs[adc_oversampling];
	gyro_data.filtered.z = (gyro_data.filtered.z * gyro_data.calibration.scale[2]) + gyro_data.calibration.bias[2];

	AttitudeRawData raw;

	raw.gyros[0] = valid_data_buffer[1];
	raw.gyros[1] = valid_data_buffer[3];
	raw.gyros[2] = valid_data_buffer[5];
	raw.gyrotemp[0] = valid_data_buffer[6];
	raw.gyrotemp[1] = valid_data_buffer[7];

	raw.gyros_filtered[0] = gyro_data.filtered.x * 180 / M_PI;
	raw.gyros_filtered[1] = gyro_data.filtered.y * 180 / M_PI;
	raw.gyros_filtered[2] = gyro_data.filtered.z * 180 / M_PI;

	raw.accels[0] = valid_data_buffer[2];
	raw.accels[1] = valid_data_buffer[0];
	raw.accels[2] = valid_data_buffer[4];

	raw.accels_filtered[0] = accel_data.filtered.x;
	raw.accels_filtered[1] = accel_data.filtered.y;
	raw.accels_filtered[2] = accel_data.filtered.z;

	raw.magnetometers[0] = mag_data.scaled.axis[0];
	raw.magnetometers[1] = mag_data.scaled.axis[1];
	raw.magnetometers[2] = mag_data.scaled.axis[2];

	AttitudeRawSet(&raw);
}

/**
 * @brief Assumes board is not moving computes biases and variances of sensors
 * @returns None
 *
 * All data is stored in global structures.  This function should be called from OP when
 * aircraft is in stable state and then the data stored to SD card.
 *
 * After this function the bias for each sensor will be the mean value.  This doesn't make
 * sense for the z accel so make sure 6 point calibration is also run and those values set
 * after these read.
 */
#define NBIAS 100
#define NVAR  500
void calibrate_sensors()
{
	int i,j;
	float accel_bias[3] = {0, 0, 0};
	float gyro_bias[3] = {0, 0, 0};
	float mag_bias[3] = {0, 0, 0};


	for (i = 0, j = 0; i < NBIAS; i++) {
		while (ahrs_state != AHRS_DATA_READY) ;
		ahrs_state = AHRS_PROCESSING;
		downsample_data();
		gyro_bias[0] += gyro_data.filtered.x / NBIAS;
		gyro_bias[1] += gyro_data.filtered.y / NBIAS;
		gyro_bias[2] += gyro_data.filtered.z / NBIAS;
		accel_bias[0] += accel_data.filtered.x / NBIAS;
		accel_bias[1] += accel_data.filtered.y / NBIAS;
		accel_bias[2] += accel_data.filtered.z / NBIAS;
		ahrs_state = AHRS_IDLE;

#if defined(PIOS_INCLUDE_HMC5843) && defined(PIOS_INCLUDE_I2C)
		if(PIOS_HMC5843_NewDataAvailable()) {
			j ++;
			PIOS_HMC5843_ReadMag(mag_data.raw.axis);
			mag_data.scaled.axis[0] = (mag_data.raw.axis[0] * mag_data.calibration.scale[0]) + mag_data.calibration.bias[0];
			mag_data.scaled.axis[1] = (mag_data.raw.axis[1] * mag_data.calibration.scale[1]) + mag_data.calibration.bias[1];
			mag_data.scaled.axis[2] = (mag_data.raw.axis[2] * mag_data.calibration.scale[2]) + mag_data.calibration.bias[2];
			mag_bias[0] += mag_data.scaled.axis[0];
			mag_bias[1] += mag_data.scaled.axis[1];
			mag_bias[2] += mag_data.scaled.axis[2];
		}
#endif

	}
	mag_bias[0] /= j;
	mag_bias[1] /= j;
	mag_bias[2] /= j;

	gyro_data.calibration.variance[0] = 0;
	gyro_data.calibration.variance[1] = 0;
	gyro_data.calibration.variance[2] = 0;
	mag_data.calibration.variance[0] = 0;
	mag_data.calibration.variance[1] = 0;
	mag_data.calibration.variance[2] = 0;
	accel_data.calibration.variance[0] = 0;
	accel_data.calibration.variance[1] = 0;
	accel_data.calibration.variance[2] = 0;

	for (i = 0, j = 0; j < NVAR; j++) {
		while (ahrs_state != AHRS_DATA_READY) ;
		ahrs_state = AHRS_PROCESSING;
		downsample_data();
		gyro_data.calibration.variance[0] += pow(gyro_data.filtered.x-gyro_bias[0],2) / NVAR;
		gyro_data.calibration.variance[1] += pow(gyro_data.filtered.y-gyro_bias[1],2) / NVAR;
		gyro_data.calibration.variance[2] += pow(gyro_data.filtered.z-gyro_bias[2],2) / NVAR;
		accel_data.calibration.variance[0] += pow(accel_data.filtered.x-accel_bias[0],2) / NVAR;
		accel_data.calibration.variance[1] += pow(accel_data.filtered.y-accel_bias[1],2) / NVAR;
		accel_data.calibration.variance[2] += pow(accel_data.filtered.z-accel_bias[2],2) / NVAR;
		ahrs_state = AHRS_IDLE;
#if defined(PIOS_INCLUDE_HMC5843) && defined(PIOS_INCLUDE_I2C)
		if(PIOS_HMC5843_NewDataAvailable()) {
			j ++;
			PIOS_HMC5843_ReadMag(mag_data.raw.axis);
			mag_data.scaled.axis[0] = (mag_data.raw.axis[0] * mag_data.calibration.scale[0]) + mag_data.calibration.bias[0];
			mag_data.scaled.axis[1] = (mag_data.raw.axis[1] * mag_data.calibration.scale[1]) + mag_data.calibration.bias[1];
			mag_data.scaled.axis[2] = (mag_data.raw.axis[2] * mag_data.calibration.scale[2]) + mag_data.calibration.bias[2];
			mag_data.calibration.variance[0] += pow(mag_data.scaled.axis[0]-mag_bias[0],2);
			mag_data.calibration.variance[1] += pow(mag_data.scaled.axis[1]-mag_bias[1],2);
			mag_data.calibration.variance[2] += pow(mag_data.scaled.axis[2]-mag_bias[2],2);
		}
#endif

	}

	mag_data.calibration.variance[0] /= j;
	mag_data.calibration.variance[1] /= j;
	mag_data.calibration.variance[2] /= j;

	gyro_data.calibration.bias[0] -= gyro_bias[0];
	gyro_data.calibration.bias[1] -= gyro_bias[1];
	gyro_data.calibration.bias[2] -= gyro_bias[2];
}

/**
 * @brief Populate fields with initial values
 */
void reset_values()
{
	accel_data.calibration.scale[0][1] = 0;
	accel_data.calibration.scale[1][0] = 0;
	accel_data.calibration.scale[0][2] = 0;
	accel_data.calibration.scale[2][0] = 0;
	accel_data.calibration.scale[1][2] = 0;
	accel_data.calibration.scale[2][1] = 0;

	accel_data.calibration.scale[0][0] = 0.036;
	accel_data.calibration.scale[1][1] = 0.036;
	accel_data.calibration.scale[2][2] = -0.036;
	accel_data.calibration.scale[0][3] = -72;
	accel_data.calibration.scale[1][3] = -72;
	accel_data.calibration.scale[2][3] = 72;

	accel_data.calibration.variance[0] = 1e-4;
	accel_data.calibration.variance[1] = 1e-4;
	accel_data.calibration.variance[2] = 1e-4;

	gyro_data.calibration.scale[0] = -0.014;
	gyro_data.calibration.scale[1] = 0.014;
	gyro_data.calibration.scale[2] = -0.014;
	gyro_data.calibration.bias[0] = -24;
	gyro_data.calibration.bias[1] = -24;
	gyro_data.calibration.bias[2] = -24;
	gyro_data.calibration.variance[0] = 1;
	gyro_data.calibration.variance[1] = 1;
	gyro_data.calibration.variance[2] = 1;
	mag_data.calibration.scale[0] = 1;
	mag_data.calibration.scale[1] = 1;
	mag_data.calibration.scale[2] = 1;
	mag_data.calibration.bias[0] = 0;
	mag_data.calibration.bias[1] = 0;
	mag_data.calibration.bias[2] = 0;
	mag_data.calibration.variance[0] = 1;
	mag_data.calibration.variance[1] = 1;
	mag_data.calibration.variance[2] = 1;
}


void send_attitude(void)
{
	AttitudeActualData attitude;
	AHRSSettingsData settings;
	AHRSSettingsGet(&settings);
	
	attitude.q1 = attitude_data.quaternion.q1;
	attitude.q2 = attitude_data.quaternion.q2;
	attitude.q3 = attitude_data.quaternion.q3;
	attitude.q4 = attitude_data.quaternion.q4;
	float rpy[3];
	Quaternion2RPY(&attitude_data.quaternion.q1, rpy);
	attitude.Roll = rpy[0] + settings.RollBias;
	attitude.Pitch = rpy[1] + settings.PitchBias;
	attitude.Yaw = rpy[2] + settings.YawBias;
	if(attitude.Yaw > 360)
		attitude.Yaw -= 360;
	AttitudeActualSet(&attitude);
}

void send_velocity(void)
{
	VelocityActualData velocityActual;
	VelocityActualGet(&velocityActual);
	
	// convert into cm
	velocityActual.North = Nav.Vel[0] * 100;
	velocityActual.East = Nav.Vel[1] * 100;
	velocityActual.Down = Nav.Vel[2] * 100;	
	
	VelocityActualSet(&velocityActual);
}

void send_position(void)
{
	PositionActualData positionActual;
	PositionActualGet(&positionActual);
	
	// convert into cm
	positionActual.North = Nav.Pos[0] * 100;
	positionActual.East = Nav.Pos[1] * 100;
	positionActual.Down = Nav.Pos[2] * 100;	
	
	PositionActualSet(&positionActual);
}

void send_calibration(void)
{
	AHRSCalibrationData cal;
	AHRSCalibrationGet(&cal);
	for(int ct=0; ct<3; ct++)
	{
		cal.accel_var[ct] = accel_data.calibration.variance[ct];
		cal.gyro_bias[ct] = gyro_data.calibration.bias[ct];
		cal.gyro_var[ct] = gyro_data.calibration.variance[ct];
		cal.mag_var[ct] = mag_data.calibration.variance[ct];
	}
	cal.measure_var = AHRSCALIBRATION_MEASURE_VAR_SET;
	AHRSCalibrationSet(&cal);
}

/**
 * @brief AHRS calibration callback
 *
 * Called when the OP board sets the calibration
 */
void calibration_callback(AhrsObjHandle obj)
{
	AHRSCalibrationData cal;
	AHRSCalibrationGet(&cal);
	if(cal.measure_var == AHRSCALIBRATION_MEASURE_VAR_SET){

		accel_data.calibration.scale[0][1] = cal.accel_ortho[0];
		accel_data.calibration.scale[1][0] = cal.accel_ortho[0];

		accel_data.calibration.scale[0][2] = cal.accel_ortho[1];
		accel_data.calibration.scale[2][0] = cal.accel_ortho[1];

		accel_data.calibration.scale[1][2] = cal.accel_ortho[2];
		accel_data.calibration.scale[2][1] = cal.accel_ortho[2];

		for(int ct=0; ct<3; ct++)
		{
			accel_data.calibration.scale[ct][ct] = cal.accel_scale[ct];
			accel_data.calibration.scale[ct][3] = cal.accel_bias[ct];
			accel_data.calibration.variance[ct] = cal.accel_var[ct];

			gyro_data.calibration.scale[ct] = cal.gyro_scale[ct];
			gyro_data.calibration.bias[ct] = cal.gyro_bias[ct];
			gyro_data.calibration.variance[ct] = cal.gyro_var[ct];

			mag_data.calibration.bias[ct] = cal.mag_bias[ct];
			mag_data.calibration.scale[ct] = cal.mag_scale[ct];
			mag_data.calibration.variance[ct] = cal.mag_var[ct];
		}
		// Note: We need the divided by 1000^2 since we scale mags to have a norm of 1000 and they are scaled to 
		// one in code
		float mag_var[3] = {mag_data.calibration.variance[0] / INSGPS_MAGLEN / INSGPS_MAGLEN, 
			mag_data.calibration.variance[1] / INSGPS_MAGLEN / INSGPS_MAGLEN, 
			mag_data.calibration.variance[2] / INSGPS_MAGLEN / INSGPS_MAGLEN};
		INSSetMagVar(mag_var);
		INSSetAccelVar(accel_data.calibration.variance);
		INSSetGyroVar(gyro_data.calibration.variance);
	}
	else if(cal.measure_var == AHRSCALIBRATION_MEASURE_VAR_MEASURE) {
		calibrate_sensors();
		send_calibration();
	}
}

void gps_callback(AhrsObjHandle obj)
{
	GPSPositionData pos;
	GPSPositionGet(&pos);
	HomeLocationData home;
	HomeLocationGet(&home);

	if((ahrs_algorithm != AHRSSETTINGS_ALGORITHM_INSGPS_OUTDOOR) ||
	   (pos.Satellites < INSGPS_GPS_MINSAT) || 
	   (pos.PDOP >= INSGPS_GPS_MINPDOP) || 
	   (home.Set == FALSE) ||
	   ((home.ECEF[0] == 0) && (home.ECEF[1] == 0) && (home.ECEF[2] == 0)))
	{
		gps_data.quality = 0;
		gps_data.updated = false;
		return;
	}

	double LLA[3] = {(double) pos.Latitude / 1e7, (double) pos.Longitude / 1e7, (double) (pos.GeoidSeparation + pos.Altitude)};
	// convert from cm back to meters
	double ECEF[3] = {(double) (home.ECEF[0] / 100), (double) (home.ECEF[1] / 100), (double) (home.ECEF[2] / 100)};
		LLA2Base(LLA, ECEF, (float (*)[3]) home.RNE, gps_data.NED);

	gps_data.heading = pos.Heading;
	gps_data.groundspeed = pos.Groundspeed;
	gps_data.quality = 1;  /* currently unused */
	gps_data.updated = true;	
}

void altitude_callback(AhrsObjHandle obj)
{
	BaroAltitudeData alt;
	BaroAltitudeGet(&alt);
	altitude_data.altitude = alt.Altitude;
	altitude_data.updated = true;
}

void settings_callback(AhrsObjHandle obj)
{
	AHRSSettingsData settings;
	AHRSSettingsGet(&settings);

	ahrs_algorithm = settings.Algorithm;
	
	if(settings.Downsampling != adc_oversampling) {
		adc_oversampling = settings.Downsampling;
		if(adc_oversampling > MAX_OVERSAMPLING) {
			adc_oversampling = MAX_OVERSAMPLING;
			settings.Downsampling = MAX_OVERSAMPLING;
			AHRSSettingsSet(&settings);
		}
		AHRS_ADC_Config(adc_oversampling);
		
		/* Use simple averaging filter for now */
		for (int i = 0; i < adc_oversampling; i++)
			fir_coeffs[i] = 1;
		fir_coeffs[adc_oversampling] = adc_oversampling;
		
	}
}

void homelocation_callback(AhrsObjHandle obj) 
{
	HomeLocationData data;
	HomeLocationGet(&data);

	float Bmag = sqrt(pow(data.Be[0],2) + pow(data.Be[1],2) + pow(data.Be[2],2));
	float Be[3] = {data.Be[0] / Bmag, data.Be[1] / Bmag, data.Be[2] / Bmag};
	
	INSSetMagNorth(Be);
}



/**
 * @}
 */
