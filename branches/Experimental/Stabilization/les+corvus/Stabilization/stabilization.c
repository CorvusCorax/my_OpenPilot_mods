/**
 ******************************************************************************
 * @addtogroup OpenPilotModules OpenPilot Modules
 * @{
 * @addtogroup StabilizationModule Stabilization Module
 * @brief Stabilization PID loops in an airframe type independent manner
 * @note This object updates the @ref ActuatorDesired "Actuator Desired" based on the
 * PID loops on the @ref AttitudeDesired "Attitude Desired" and @ref AttitudeActual "Attitude Actual"
 * @{
 *
 * @file       stabilization.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Attitude stabilization module.
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
#include "stabilization.h"
#include "stabilizationsettings.h"
#include "actuatordesired.h"
#include "ratedesired.h"
#include "attitudedesired.h"
#include "attitudeactual.h"
#include "attituderaw.h"
#include "manualcontrolcommand.h"
#include "systemsettings.h"
#include "ahrssettings.h"

// Private constants
#define MAX_QUEUE_SIZE 2
#define STACK_SIZE configMINIMAL_STACK_SIZE
#define TASK_PRIORITY (tskIDLE_PRIORITY+4)
#define FAILSAFE_TIMEOUT_MS 30
#define DEG2RAD ( M_PI / 180.0 )


// Stabilizisation variant
#define TRANSLATE_COORDS 1
// 0  <-- no coordinate translation - old behaviour - no rotation
// 1  <-- suggestion by corvus - rotate attitude error into local reference frame
// 2  <-- rotate rate error into local reference frame
// 3  <-- rotate actuator demands into local reference frame
// WARNING: MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_RATE
//          will behave differently depending on whether and when translation takes place
//          "none" and "attitude"   - rates will be stabilized in local reference frame
//          "rates" and "actuators" - rates will be stabilized in global reference frame

#define TRANSLATE_METHOD 0
// WARNING: experimental feature 1 (full) is untested!


enum {PID_RATE_ROLL, PID_RATE_PITCH, PID_RATE_YAW, PID_ROLL, PID_PITCH, PID_YAW, PID_MAX};

enum {ROLL,PITCH,YAW,MAX_AXES};

// Private types
typedef struct {
	float p;
	float i;
	float d;
	float iLim;
	float iAccumulator;
	float lastErr;
} pid_type;

// Private variables
static xTaskHandle taskHandle;
static StabilizationSettingsData settings;
static xQueueHandle queue;
float dT = 1;
pid_type pids[PID_MAX];

// Private functions
static void stabilizationTask(void* parameters);
static float ApplyPid(pid_type * pid, const float error);
static float bound(float val);
static void ZeroPids(void);
static void SettingsUpdatedCb(UAVObjEvent * ev);
static void calcDifference(float * values, float * reference, const uint8_t angular);
static void translateValues(float * values, float * reference);

// Global updated variable
volatile uint8_t stabilization_updated;

/**
 * Module initialization
 */
int32_t StabilizationInitialize()
{
	// Initialize variables

	// Create object queue
	queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(UAVObjEvent));

	// Listen for updates.
	AttitudeActualConnectQueue(queue);
	//AttitudeRawConnectQueue(queue);

	StabilizationSettingsConnectCallback(SettingsUpdatedCb);
	SettingsUpdatedCb(StabilizationSettingsHandle());
	// Start main task
	xTaskCreate(stabilizationTask, (signed char*)"Stabilization", STACK_SIZE, NULL, TASK_PRIORITY, &taskHandle);

	return 0;
}


/**
 * Module task
 */
static void stabilizationTask(void* parameters)
{
	portTickType lastSysTime;
	portTickType lastSleepSysTime;
	portTickType thisSysTime;
	UAVObjEvent ev;


	ActuatorDesiredData actuatorDesired;
	AttitudeDesiredData attitudeDesired;
	RateDesiredData rateDesired;
	AttitudeActualData attitudeActual;
	AttitudeRawData attitudeRaw;
	SystemSettingsData systemSettings;
	ManualControlCommandData manualControl;

	SettingsUpdatedCb((UAVObjEvent *) NULL);

	// Main task loop
	lastSysTime = xTaskGetTickCount();
	lastSleepSysTime = lastSysTime;
	ZeroPids();
	while(1) {
		// Wait until the ActuatorDesired object is updated, if a timeout then go to failsafe
		if ( xQueueReceive(queue, &ev, FAILSAFE_TIMEOUT_MS / portTICK_RATE_MS) != pdTRUE )
		{
			AlarmsSet(SYSTEMALARMS_ALARM_STABILIZATION,SYSTEMALARMS_ALARM_WARNING);
		}

		stabilization_updated = 1;

		// Check how long since last update
		thisSysTime = xTaskGetTickCount();
		if(thisSysTime > lastSysTime) // reuse dt in case of wraparound
			dT = (thisSysTime - lastSysTime) / portTICK_RATE_MS / 1000.0f;
		lastSysTime = thisSysTime;

		ManualControlCommandGet(&manualControl);
		AttitudeDesiredGet(&attitudeDesired);
		AttitudeActualGet(&attitudeActual);
		AttitudeRawGet(&attitudeRaw);
		RateDesiredGet(&rateDesired);
		SystemSettingsGet(&systemSettings);


		float *manualAxis = &manualControl.Roll;
		float *attitudeDesiredAxis = &attitudeDesired.Roll;
		float *attitudeActualAxis = &attitudeActual.Roll;
		float *actuatorDesiredAxis = &actuatorDesired.Roll;
		float *rateDesiredAxis = &rateDesired.Roll;

		// calculate attitude errors
		calcDifference( attitudeDesiredAxis, attitudeActualAxis, 1);

		//Zero errors we are not using
		for (int8_t ct=0; ct< MAX_AXES; ct++)
		{
			if ( manualControl.StabilizationSettings[ct] != MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_ATTITUDE )
			{
				attitudeDesiredAxis[ct] = 0;
			}
			if ( manualControl.StabilizationSettings[ct] != MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_RATE )
			{
				manualAxis[ct] = 0;
			}
		}

#if TRANSLATE_COORDS == 1
		//Translate Attitude to local reference frame.
		translateValues(attitudeDesiredAxis, attitudeActualAxis);
		translateValues(manualAxis, attitudeActualAxis);
#endif

		//Calculate desired rate
		for(int8_t ct=0; ct< MAX_AXES; ct++)
		{
			rateDesiredAxis[ct] = manualAxis[ct] * settings.ManualRate[ct]
								+ ApplyPid(&pids[PID_ROLL + ct],  attitudeDesiredAxis[ct]);
			if(fabs(rateDesiredAxis[ct]) > settings.MaximumRate[ct])
			{
				if(rateDesiredAxis[ct] > 0)
				{
					rateDesiredAxis[ct] = settings.MaximumRate[ct];
				}else
				{
					rateDesiredAxis[ct] = -settings.MaximumRate[ct];
				}
			}
		}

		uint8_t shouldUpdate = 0;
		RateDesiredSet(&rateDesired);
		ActuatorDesiredGet(&actuatorDesired);

		// calculate rate errors
		calcDifference( rateDesiredAxis, attitudeRaw.gyros_filtered, 0);

#if TRANSLATE_COORDS == 2
		//Translate rate errors to local reference frame.
		translateValues(rateDesiredAxis, attitudeActualAxis);
		#warning this is untested
#endif

		//Calculate desired command
		for(int8_t ct=0; ct< MAX_AXES; ct++)
		{
			switch(manualControl.StabilizationSettings[ct])
			{
			case MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_RATE:
			case MANUALCONTROLCOMMAND_STABILIZATIONSETTINGS_ATTITUDE:
				{
					float command = ApplyPid(&pids[PID_RATE_ROLL + ct],  rateDesiredAxis[ct]);
					actuatorDesiredAxis[ct] = bound(command);
					shouldUpdate = 1;
					break;
				}
			}
		}

#if TRANSLATE_COORDS == 3
		//Translate Actuator settings to local reference frame.
		translateValues(actuatorDesiredAxis,attitudeActualAxis);
		#warning this is untested
#endif

		// Save dT
		actuatorDesired.UpdateTime = dT * 1000;

		if(manualControl.FlightMode == MANUALCONTROLCOMMAND_FLIGHTMODE_MANUAL)
		{
			shouldUpdate = 0;
		}


		if(shouldUpdate)
		{
			actuatorDesired.Throttle = attitudeDesired.Throttle;
			ActuatorDesiredSet(&actuatorDesired);
		}

		if(manualControl.Armed == MANUALCONTROLCOMMAND_ARMED_FALSE ||
			!shouldUpdate || (attitudeDesired.Throttle < 0))
		{
			ZeroPids();
		}

		// Clear alarms
		AlarmsClear(SYSTEMALARMS_ALARM_STABILIZATION);

		/* Wait for the next update interval */
		vTaskDelayUntil(&lastSleepSysTime, settings.UpdatePeriod / portTICK_RATE_MS);
	}
}


float ApplyPid(pid_type * pid, const float err)
{
	float diff = (err - pid->lastErr);
	pid->lastErr = err;
	pid->iAccumulator += err * pid->i * dT;
	if (isnan(pid->iAccumulator)) pid->iAccumulator = 0;
	if(fabs(pid->iAccumulator) > pid->iLim) {
		if(pid->iAccumulator >0) {
			pid->iAccumulator = pid->iLim;
		} else {
			pid->iAccumulator = -pid->iLim;
		}
	}
	return ((err * pid->p) + pid->iAccumulator + (diff * pid->d / dT));
}


static void ZeroPids(void)
{
	for(int8_t ct = 0; ct < PID_MAX; ct++) {
		pids[ct].iAccumulator = 0;
		pids[ct].lastErr = 0;
	}
}


/**
 * Bound input value between limits
 */
static float bound(float val)
{
	if(val < -1) {
		val = -1;
	} else if(val > 1) {
		val = 1;
	}
	if (isnan(val)) return 0;
	return val;
}


/**
 * calculate difference vector
 */
static void calcDifference(float * values, float * reference, const uint8_t angular)
{
	for(int8_t ct=0; ct< MAX_AXES; ct++)
	{
		values[ct] = values[ct] - reference[ct];
		if (angular) {
			if (values[ct] >  180.) values[ct] -= 360.;
			if (values[ct] < -180.) values[ct] += 360.;
		}
	}
}


/**
 * translate rotational vector into local reference frame
 */
static void translateValues(float * values, float * reference)
{

	float tmp[MAX_AXES];

#if TRANSLATE_METHOD == 1 && TRANSLATE_COORDS == 1
	// UNTESTED!!! (and also likely unnecessary since neglectible for small values)
	// adjust PITCH to corect the (PITCH) difference between a YAW rotation around the
	// vertical axis and a YAW rotation around the local vertical axis
	// WARNING!!! This only makes sense if values[YAW] is an angle.
	// Therefore it cannot work for rates and/or actuatorDemands
	values[PITCH] = values[PITCH] + ( reference[PITCH] - cos( DEG2RAD * values[YAW] ) * reference[PITCH] );
	#warning this is untested
#endif

	// traditional translation: rotate YAW and PITCH by roll
	tmp[PITCH] = cos( DEG2RAD * reference[ROLL] ) * values[PITCH]
			+ sin( DEG2RAD * reference[ROLL] ) * values[YAW];
	tmp[YAW]   = cos( DEG2RAD * reference[ROLL] ) * values[YAW]
			- sin( DEG2RAD * reference[ROLL] ) * values[PITCH];
	values[PITCH] = tmp[PITCH];
	values[YAW]   = tmp[YAW];

}

static void SettingsUpdatedCb(UAVObjEvent * ev)
{
	memset(pids,0,sizeof (pid_type) * PID_MAX);
	StabilizationSettingsGet(&settings);

	float * data = settings.RollRatePI;
	for(int8_t pid=0; pid < PID_MAX; pid++)
	{
		pids[pid].p = *data++;
		pids[pid].i = *data++;
		pids[pid].iLim = *data++;
	}
}


/**
  * @}
  * @}
  */
