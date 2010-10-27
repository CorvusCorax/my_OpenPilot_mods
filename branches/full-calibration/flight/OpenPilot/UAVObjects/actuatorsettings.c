/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup ActuatorSettings ActuatorSettings
 * @brief Settings for the @ref ActuatorModule that controls the channel assignments for the mixer based on AircraftType
 *
 * Autogenerated files and functions for ActuatorSettings Object
 * @{ 
 *
 * @file       actuatorsettings.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the ActuatorSettings object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: actuatorsettings.xml. 
 *             This is an automatically generated file.
 *             DO NOT modify manually.
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
#include "actuatorsettings.h"

// Private variables
static UAVObjHandle handle;

// Private functions
static void setDefaults(UAVObjHandle obj, uint16_t instId);

/**
 * Initialize object.
 * \return 0 Success
 * \return -1 Failure
 */
int32_t ActuatorSettingsInitialize()
{
	// Register object with the object manager
	handle = UAVObjRegister(ACTUATORSETTINGS_OBJID, ACTUATORSETTINGS_NAME, ACTUATORSETTINGS_METANAME, 0,
			ACTUATORSETTINGS_ISSINGLEINST, ACTUATORSETTINGS_ISSETTINGS, ACTUATORSETTINGS_NUMBYTES, &setDefaults);

	// Done
	if (handle != 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/**
 * Initialize object fields and metadata with the default values.
 * If a default value is not specified the object fields
 * will be initialized to zero.
 */
static void setDefaults(UAVObjHandle obj, uint16_t instId)
{
	ActuatorSettingsData data;
	UAVObjMetadata metadata;

	// Initialize object fields to their default values
	UAVObjGetInstanceData(obj, instId, &data);
	memset(&data, 0, sizeof(ActuatorSettingsData));
	data.FixedWingRoll1 = 8;
	data.FixedWingRoll2 = 8;
	data.FixedWingPitch1 = 8;
	data.FixedWingPitch2 = 8;
	data.FixedWingYaw = 8;
	data.FixedWingThrottle = 8;
	data.VTOLMotorN = 8;
	data.VTOLMotorNE = 8;
	data.VTOLMotorE = 8;
	data.VTOLMotorSE = 8;
	data.VTOLMotorS = 8;
	data.VTOLMotorSW = 8;
	data.VTOLMotorW = 8;
	data.VTOLMotorNW = 8;
	data.CCPMYawStabilizationInManualMode = 0;
	data.CCPMFlybarless = 0;
	data.CCPMThrottleCurve[0] = 0;
	data.CCPMThrottleCurve[1] = 0;
	data.CCPMThrottleCurve[2] = 0;
	data.CCPMThrottleCurve[3] = 0;
	data.CCPMThrottleCurve[4] = 0;
	data.CCPMPitchCurve[0] = 0;
	data.CCPMPitchCurve[1] = 0;
	data.CCPMPitchCurve[2] = 0;
	data.CCPMPitchCurve[3] = 0;
	data.CCPMPitchCurve[4] = 0;
	data.CCPMCollectiveConstant = 0.5;
	data.CCPMCorrectionAngle = 0;
	data.CCPMAngleW = 0;
	data.CCPMAngleX = 60;
	data.CCPMAngleY = 180;
	data.CCPMAngleZ = 300;
	data.CCPMServoW = 8;
	data.CCPMServoX = 8;
	data.CCPMServoY = 8;
	data.CCPMServoZ = 8;
	data.CCPMThrottle = 8;
	data.CCPMTailRotor = 8;
	data.ChannelUpdateFreq[0] = 50;
	data.ChannelUpdateFreq[1] = 50;
	data.ChannelMax[0] = 1000;
	data.ChannelMax[1] = 1000;
	data.ChannelMax[2] = 1000;
	data.ChannelMax[3] = 1000;
	data.ChannelMax[4] = 1000;
	data.ChannelMax[5] = 1000;
	data.ChannelMax[6] = 1000;
	data.ChannelMax[7] = 1000;
	data.ChannelNeutral[0] = 1000;
	data.ChannelNeutral[1] = 1000;
	data.ChannelNeutral[2] = 1000;
	data.ChannelNeutral[3] = 1000;
	data.ChannelNeutral[4] = 1000;
	data.ChannelNeutral[5] = 1000;
	data.ChannelNeutral[6] = 1000;
	data.ChannelNeutral[7] = 1000;
	data.ChannelMin[0] = 1000;
	data.ChannelMin[1] = 1000;
	data.ChannelMin[2] = 1000;
	data.ChannelMin[3] = 1000;
	data.ChannelMin[4] = 1000;
	data.ChannelMin[5] = 1000;
	data.ChannelMin[6] = 1000;
	data.ChannelMin[7] = 1000;

	UAVObjSetInstanceData(obj, instId, &data);

	// Initialize object metadata to their default values
	metadata.access = ACCESS_READWRITE;
	metadata.gcsAccess = ACCESS_READWRITE;
	metadata.telemetryAcked = 1;
	metadata.telemetryUpdateMode = UPDATEMODE_ONCHANGE;
	metadata.telemetryUpdatePeriod = 0;
	metadata.gcsTelemetryAcked = 1;
	metadata.gcsTelemetryUpdateMode = UPDATEMODE_ONCHANGE;
	metadata.gcsTelemetryUpdatePeriod = 0;
	metadata.loggingUpdateMode = UPDATEMODE_NEVER;
	metadata.loggingUpdatePeriod = 0;
	UAVObjSetMetadata(obj, &metadata);
}

/**
 * Get object handle
 */
UAVObjHandle ActuatorSettingsHandle()
{
	return handle;
}

/**
 * @}
 */

