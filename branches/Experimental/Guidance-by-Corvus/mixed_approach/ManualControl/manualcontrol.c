/**
 ******************************************************************************
 * @addtogroup OpenPilotModules OpenPilot Modules
 * @{
 * @addtogroup ManualControlModule Manual Control Module
 * @brief Provide manual control or allow it alter flight mode.
 * @{
 *
 * Reads in the ManualControlCommand FlightMode setting from receiver then either
 * pass the settings straght to ActuatorDesired object (manual mode) or to
 * AttitudeDesired and RateDesired object (stabilized mode)
 *
 * @file       manualcontrol.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      ManualControl module. Handles safety R/C link and flight mode.
 *
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

#include "openpilot.h"
#include "manualcontrol.h"
#include "manualcontrolsettings.h"
#include "stabilizationsettings.h"
#include "manualcontrolcommand.h"
#include "actuatordesired.h"
#include "attitudedesired.h"
#include "ratedesired.h"
#include "flighttelemetrystats.h"

// Private constants
#if defined(PIOS_MANUAL_STACK_SIZE)
#define STACK_SIZE_BYTES PIOS_MANUAL_STACK_SIZE
#else
#define STACK_SIZE_BYTES 824
#endif

#define TASK_PRIORITY (tskIDLE_PRIORITY+4)
#define UPDATE_PERIOD_MS 20
#define THROTTLE_FAILSAFE -0.1
#define FLIGHT_MODE_LIMIT 1.0/3.0
#define ARMED_TIME_MS      1000
//safe band to allow a bit of calibration error or trim offset (in microseconds)
#define CONNECTION_OFFSET 150

// Private types
typedef enum
{
	ARM_STATE_DISARMED,
	ARM_STATE_ARMING_MANUAL,
	ARM_STATE_ARMED,
	ARM_STATE_DISARMING_MANUAL,
	ARM_STATE_DISARMING_TIMEOUT
} ArmState_t;

// Private variables
static xTaskHandle taskHandle;
static ArmState_t armState;

// Private functions
static void manualControlTask(void *parameters);
static float scaleChannel(int16_t value, int16_t max, int16_t min, int16_t neutral);
static uint32_t timeDifferenceMs(portTickType start_time, portTickType end_time);

#define assumptions1 ( \
		((int)MANUALCONTROLSETTINGS_POS1STABILIZATIONSETTINGS_NONE 		== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_NONE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS1STABILIZATIONSETTINGS_DIRECT 		== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_DIRECT) 		&& \
		((int)MANUALCONTROLSETTINGS_POS1STABILIZATIONSETTINGS_RATE 		== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_RATE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS1STABILIZATIONSETTINGS_GLOBALRATE	== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_GLOBALRATE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS1STABILIZATIONSETTINGS_ATTITUDE	== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_ATTITUDE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS1STABILIZATIONSETTINGS_MIXED 	== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_MIXED) 	   \
		)

#define assumptions2 ( \
		((int)MANUALCONTROLSETTINGS_POS1FLIGHTMODE_MANUAL 			== (int)MANUALCONTROLCOMMAND_FLIGHTMODE_MANUAL) 						&& \
		((int)MANUALCONTROLSETTINGS_POS1FLIGHTMODE_STABILIZED		== (int)MANUALCONTROLCOMMAND_FLIGHTMODE_STABILIZED) 					&& \
		((int)MANUALCONTROLSETTINGS_POS1FLIGHTMODE_AUTO 				== (int)MANUALCONTROLCOMMAND_FLIGHTMODE_AUTO) 						   \
		)

#define assumptions3 ( \
		((int)MANUALCONTROLSETTINGS_POS2STABILIZATIONSETTINGS_NONE 		== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_NONE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS2STABILIZATIONSETTINGS_DIRECT 		== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_DIRECT) 		&& \
		((int)MANUALCONTROLSETTINGS_POS2STABILIZATIONSETTINGS_RATE 		== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_RATE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS2STABILIZATIONSETTINGS_GLOBALRATE	== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_GLOBALRATE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS2STABILIZATIONSETTINGS_ATTITUDE	== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_ATTITUDE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS2STABILIZATIONSETTINGS_MIXED 	== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_MIXED) 	   \
		)

#define assumptions4 ( \
		((int)MANUALCONTROLSETTINGS_POS2FLIGHTMODE_MANUAL 			== (int)MANUALCONTROLCOMMAND_FLIGHTMODE_MANUAL) 						&& \
		((int)MANUALCONTROLSETTINGS_POS2FLIGHTMODE_STABILIZED		== (int)MANUALCONTROLCOMMAND_FLIGHTMODE_STABILIZED) 					&& \
		((int)MANUALCONTROLSETTINGS_POS2FLIGHTMODE_AUTO 				== (int)MANUALCONTROLCOMMAND_FLIGHTMODE_AUTO) 						   \
		)

#define assumptions5 ( \
		((int)MANUALCONTROLSETTINGS_POS3STABILIZATIONSETTINGS_NONE 		== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_NONE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS3STABILIZATIONSETTINGS_DIRECT 		== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_DIRECT) 		&& \
		((int)MANUALCONTROLSETTINGS_POS3STABILIZATIONSETTINGS_RATE 		== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_RATE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS3STABILIZATIONSETTINGS_GLOBALRATE	== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_GLOBALRATE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS3STABILIZATIONSETTINGS_ATTITUDE	== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_ATTITUDE) 		&& \
		((int)MANUALCONTROLSETTINGS_POS3STABILIZATIONSETTINGS_MIXED 	== (int)MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_MIXED) 	   \
		)

#define assumptions6 ( \
		((int)MANUALCONTROLSETTINGS_POS3FLIGHTMODE_MANUAL 			== (int)MANUALCONTROLCOMMAND_FLIGHTMODE_MANUAL) 						&& \
		((int)MANUALCONTROLSETTINGS_POS3FLIGHTMODE_STABILIZED		== (int)MANUALCONTROLCOMMAND_FLIGHTMODE_STABILIZED) 					&& \
		((int)MANUALCONTROLSETTINGS_POS3FLIGHTMODE_AUTO 				== (int)MANUALCONTROLCOMMAND_FLIGHTMODE_AUTO) 						   \
		)



#define assumptions (assumptions1 && assumptions2 && assumptions3 && assumptions4 && assumptions5 && assumptions6)

/**
 * Module initialization
 */
int32_t ManualControlInitialize()
{
	// The following line compiles to nothing when the assumptions are correct
	if (!assumptions)
		return 1;
	// In case the assumptions are incorrect, this module will not initialise and so will not function

	// Start main task
	xTaskCreate(manualControlTask, (signed char *)"ManualControl", STACK_SIZE_BYTES/4, NULL, TASK_PRIORITY, &taskHandle);
	TaskMonitorAdd(TASKINFO_RUNNING_MANUALCONTROL, taskHandle);
	PIOS_WDG_RegisterFlag(PIOS_WDG_MANUAL);


	return 0;
}

/**
 * Module task
 */
static void manualControlTask(void *parameters)
{
	ManualControlSettingsData settings;
	StabilizationSettingsData stabSettings;
	ManualControlCommandData cmd;
	ActuatorDesiredData actuator;
	AttitudeDesiredData attitude;
	RateDesiredData rate;
	portTickType lastSysTime;
	

	float flightMode;

	uint8_t disconnected_count = 0;
	uint8_t connected_count = 0;
	enum { CONNECTED, DISCONNECTED } connection_state = DISCONNECTED;

	// Make sure unarmed on power up
	ManualControlCommandGet(&cmd);
	cmd.Armed = MANUALCONTROLCOMMAND_ARMED_FALSE;
	ManualControlCommandSet(&cmd);
	armState = ARM_STATE_DISARMED;

	// Main task loop
	lastSysTime = xTaskGetTickCount();
	while (1) {
		float scaledChannel[MANUALCONTROLCOMMAND_CHANNEL_NUMELEM];

		// Wait until next update
		vTaskDelayUntil(&lastSysTime, UPDATE_PERIOD_MS / portTICK_RATE_MS);
		PIOS_WDG_UpdateFlag(PIOS_WDG_MANUAL);
		
		// Read settings
		ManualControlSettingsGet(&settings);
		StabilizationSettingsGet(&stabSettings);

		if (ManualControlCommandReadOnly(&cmd)) {
			FlightTelemetryStatsData flightTelemStats;
			FlightTelemetryStatsGet(&flightTelemStats);
			if(flightTelemStats.Status != FLIGHTTELEMETRYSTATS_STATUS_CONNECTED) { 
				/* trying to fly via GCS and lost connection.  fall back to transmitter */
				UAVObjMetadata metadata;
				UAVObjGetMetadata(&cmd, &metadata);
				metadata.access = ACCESS_READWRITE;
				UAVObjSetMetadata(&cmd, &metadata);				
			}
		}
			
		if (!ManualControlCommandReadOnly(&cmd)) {

			// Check settings, if error raise alarm
			if (settings.Roll >= MANUALCONTROLSETTINGS_ROLL_NONE ||
				settings.Pitch >= MANUALCONTROLSETTINGS_PITCH_NONE ||
				settings.Yaw >= MANUALCONTROLSETTINGS_YAW_NONE ||
				settings.Throttle >= MANUALCONTROLSETTINGS_THROTTLE_NONE ||
				settings.FlightMode >= MANUALCONTROLSETTINGS_FLIGHTMODE_NONE) {
				AlarmsSet(SYSTEMALARMS_ALARM_MANUALCONTROL, SYSTEMALARMS_ALARM_CRITICAL);
				cmd.FlightMode = MANUALCONTROLCOMMAND_FLIGHTMODE_AUTO;
				cmd.Connected = MANUALCONTROLCOMMAND_CONNECTED_FALSE;
				ManualControlCommandSet(&cmd);
				continue;
			}
			// Read channel values in us
			// TODO: settings.InputMode is currently ignored because PIOS will not allow runtime
			// selection of PWM and PPM. The configuration is currently done at compile time in
			// the pios_config.h file.
			for (int n = 0; n < MANUALCONTROLCOMMAND_CHANNEL_NUMELEM; ++n) {
#if defined(PIOS_INCLUDE_PWM)
				cmd.Channel[n] = PIOS_PWM_Get(n);
#elif defined(PIOS_INCLUDE_PPM)
				cmd.Channel[n] = PIOS_PPM_Get(n);
#elif defined(PIOS_INCLUDE_SPEKTRUM)
				cmd.Channel[n] = PIOS_SPEKTRUM_Get(n);
#endif
				scaledChannel[n] = scaleChannel(cmd.Channel[n], settings.ChannelMax[n],	settings.ChannelMin[n], settings.ChannelNeutral[n]);
			}

			// Scale channels to -1 -> +1 range
			cmd.Roll 		= scaledChannel[settings.Roll];
			cmd.Pitch 		= scaledChannel[settings.Pitch];
			cmd.Yaw 		= scaledChannel[settings.Yaw];
			cmd.Throttle 	= scaledChannel[settings.Throttle];
			flightMode 		= scaledChannel[settings.FlightMode];

			if (settings.Accessory1 != MANUALCONTROLSETTINGS_ACCESSORY1_NONE)
				cmd.Accessory1 = scaledChannel[settings.Accessory1];
			else
				cmd.Accessory1 = 0;

			if (settings.Accessory2 != MANUALCONTROLSETTINGS_ACCESSORY2_NONE)
				cmd.Accessory2 = scaledChannel[settings.Accessory2];
			else
				cmd.Accessory2 = 0;

			if (settings.Accessory3 != MANUALCONTROLSETTINGS_ACCESSORY3_NONE)
				cmd.Accessory3 = scaledChannel[settings.Accessory3];
			else
				cmd.Accessory3 = 0;

			if (flightMode < -FLIGHT_MODE_LIMIT) {
				// Position 1
				for(int i = 0; i < 3; i++) {
					cmd.StabilizationSettings[i] = settings.Pos1StabilizationSettings[i];	// See assumptions1
				}
				cmd.FlightMode = settings.Pos1FlightMode;	// See assumptions2
			} else if (flightMode > FLIGHT_MODE_LIMIT) {
				// Position 3
				for(int i = 0; i < 3; i++) {
					cmd.StabilizationSettings[i] = settings.Pos3StabilizationSettings[i];	// See assumptions5
				}
				cmd.FlightMode = settings.Pos3FlightMode;	// See assumptions6
			} else {
				// Position 2
				for(int i = 0; i < 3; i++) {
					cmd.StabilizationSettings[i] = settings.Pos2StabilizationSettings[i];	// See assumptions3
				}
				cmd.FlightMode = settings.Pos2FlightMode;	// See assumptions4
			}
			// Update the ManualControlCommand object
			ManualControlCommandSet(&cmd);
			// This seems silly to set then get, but the reason is if the GCS is
			// the control input, the set command will be blocked by the read only
			// setting and the get command will pull the right values from telemetry
		} else
			ManualControlCommandGet(&cmd);	/* Under GCS control */
		

		// Implement hysteresis loop on connection status
		// Must check both Max and Min in case they reversed
		if (!ManualControlCommandReadOnly(&cmd) &&
			cmd.Channel[settings.Throttle] < settings.ChannelMax[settings.Throttle] - CONNECTION_OFFSET &&
			cmd.Channel[settings.Throttle] < settings.ChannelMin[settings.Throttle] - CONNECTION_OFFSET) {
			if (disconnected_count++ > 10) {
				connection_state = DISCONNECTED;
				connected_count = 0;
				disconnected_count = 0;
			} else
				disconnected_count++;
		} else {
			if (connected_count++ > 10) {
				connection_state = CONNECTED;
				connected_count = 0;
				disconnected_count = 0;
			} else
				connected_count++;
		}

		if (connection_state == DISCONNECTED) {
			cmd.Connected = MANUALCONTROLCOMMAND_CONNECTED_FALSE;
			cmd.Throttle = -1;	// Shut down engine with no control
			cmd.Roll = 0;
			cmd.Yaw = 0;
			cmd.Pitch = 0;
			//cmd.FlightMode = MANUALCONTROLCOMMAND_FLIGHTMODE_AUTO; // don't do until AUTO implemented and functioning
			AlarmsSet(SYSTEMALARMS_ALARM_MANUALCONTROL, SYSTEMALARMS_ALARM_WARNING);
			ManualControlCommandSet(&cmd);
		} else {
			cmd.Connected = MANUALCONTROLCOMMAND_CONNECTED_TRUE;
			AlarmsClear(SYSTEMALARMS_ALARM_MANUALCONTROL);
			ManualControlCommandSet(&cmd);
		} 

		// Arming and Disarming mechanism
		if (cmd.Throttle < 0) {
			// Throttle is low, in this condition the arming state could change

			uint8_t newCmdArmed = cmd.Armed;
			static portTickType armedDisarmStart;

			// Look for state changes and write in newArmState
			if (settings.Arming == MANUALCONTROLSETTINGS_ARMING_NONE) {
				// No channel assigned to arming -> arm immediately when throttle is low
				newCmdArmed = MANUALCONTROLCOMMAND_ARMED_TRUE;
			} else {
				float armStickLevel;
				uint8_t channel = settings.Arming/2;    // 0=Channel1, 1=Channel1_Rev, 2=Channel2, ....
				bool reverse = (settings.Arming%2)==1;
				bool manualArm = false;
				bool manualDisarm = false;

				if (connection_state == CONNECTED) {
					// Should use RC input only if RX is connected
					armStickLevel = scaledChannel[channel];
					if (reverse)
						armStickLevel =-armStickLevel;

					if (armStickLevel <= -0.90)
						manualArm = true;
					else if (armStickLevel >= +0.90)
						manualDisarm = true;
				}

				switch(armState) {
				case ARM_STATE_DISARMED:
					newCmdArmed = MANUALCONTROLCOMMAND_ARMED_FALSE;
					if (manualArm) {
						armedDisarmStart = lastSysTime;
						armState = ARM_STATE_ARMING_MANUAL;
					}
					break;

				case ARM_STATE_ARMING_MANUAL:
					if (manualArm) {
						if (timeDifferenceMs(armedDisarmStart, lastSysTime) > ARMED_TIME_MS)
							armState = ARM_STATE_ARMED;
					}
					else
						armState = ARM_STATE_DISARMED;
					break;

				case ARM_STATE_ARMED:
					// When we get here, the throttle is low,
					// we go immediately to disarming due to timeout, also when the disarming mechanism is not enabled
					armedDisarmStart = lastSysTime;
					armState = ARM_STATE_DISARMING_TIMEOUT;
					newCmdArmed = MANUALCONTROLCOMMAND_ARMED_TRUE;
					break;

				case ARM_STATE_DISARMING_TIMEOUT:
					// We get here when armed while throttle low, even when the arming timeout is not enabled
					if (settings.ArmedTimeout != 0)
						if (timeDifferenceMs(armedDisarmStart, lastSysTime) > settings.ArmedTimeout)
							armState = ARM_STATE_DISARMED;
					// Switch to disarming due to manual control when needed
					if (manualDisarm) {
						armedDisarmStart = lastSysTime;
						armState = ARM_STATE_DISARMING_MANUAL;
					}
					break;

				case ARM_STATE_DISARMING_MANUAL:
					if (manualDisarm) {
						if (timeDifferenceMs(armedDisarmStart, lastSysTime) > ARMED_TIME_MS)
							armState = ARM_STATE_DISARMED;
					}
					else
						armState = ARM_STATE_ARMED;
					break;
				}
			}
			// Update cmd object when needed
			if (newCmdArmed != cmd.Armed) {
				cmd.Armed = newCmdArmed;
				ManualControlCommandSet(&cmd);
			}
		} else {
			// The throttle is not low, in case we where arming or disarming, abort
			switch(armState) {
				case ARM_STATE_DISARMING_MANUAL:
				case ARM_STATE_DISARMING_TIMEOUT:
					armState = ARM_STATE_ARMED;
					break;
				case ARM_STATE_ARMING_MANUAL:
					armState = ARM_STATE_DISARMED;
					break;
				default:
					// Nothing needs to be done in the other states
					break;
			}
		}
		// End of arming/disarming



		// Depending on the mode update the Stabilization or Actuator objects
		if (cmd.FlightMode == MANUALCONTROLCOMMAND_FLIGHTMODE_MANUAL) {
			actuator.Roll = cmd.Roll;
			actuator.Pitch = cmd.Pitch;
			actuator.Yaw = cmd.Yaw;
			actuator.Throttle = cmd.Throttle;
			ActuatorDesiredSet(&actuator);
		} else if (cmd.FlightMode == MANUALCONTROLCOMMAND_FLIGHTMODE_STABILIZED) {
			attitude.Roll = cmd.Roll * stabSettings.RollMax;
			attitude.Pitch = cmd.Pitch * stabSettings.PitchMax;
			attitude.Yaw = fmod(cmd.Yaw * 180.0, 360);
			attitude.Throttle =  (cmd.Throttle < 0) ? -1 : cmd.Throttle;
			rate.Roll = cmd.Roll * stabSettings.ManualRate[STABILIZATIONSETTINGS_MANUALRATE_ROLL];
			rate.Pitch = cmd.Pitch * stabSettings.ManualRate[STABILIZATIONSETTINGS_MANUALRATE_PITCH];
			rate.Yaw = cmd.Yaw * stabSettings.ManualRate[STABILIZATIONSETTINGS_MANUALRATE_YAW];

			AttitudeDesiredSet(&attitude);
			RateDesiredSet(&rate);
		}
	}
}

/**
 * Convert channel from servo pulse duration (microseconds) to scaled -1/+1 range.
 */
static float scaleChannel(int16_t value, int16_t max, int16_t min, int16_t neutral)
{
	float valueScaled;
	// Scale
	if ((max > min && value >= neutral) || (min > max && value <= neutral)) {
		if (max != neutral) {
			valueScaled = (float)(value - neutral) / (float)(max - neutral);
		} else {
			valueScaled = 0;
		}
	} else {
		if (min != neutral) {
			valueScaled = (float)(value - neutral) / (float)(neutral - min);
		} else {
			valueScaled = 0;
		}
	}
	// Bound
	if (valueScaled > 1.0) {
		valueScaled = 1.0;
	} else if (valueScaled < -1.0) {
		valueScaled = -1.0;
	}
	return valueScaled;
}

static uint32_t timeDifferenceMs(portTickType start_time, portTickType end_time) {
	if(end_time > start_time) 
		return (end_time - start_time) * portTICK_RATE_MS;
	return ((((portTICK_RATE_MS) -1) - start_time) + end_time) * portTICK_RATE_MS;		
}
			   

/**
  * @}
  * @}
  */
