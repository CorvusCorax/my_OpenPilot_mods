/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup GuidanceSettings GuidanceSettings
 * @brief Settings for the @ref GuidanceModule
 *
 * Autogenerated files and functions for GuidanceSettings Object
 * @{ 
 *
 * @file       guidancesettings.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the GuidanceSettings object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: guidancesettings.xml. 
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
#include "guidancesettings.h"

// Private variables
static UAVObjHandle handle;

// Private functions
static void setDefaults(UAVObjHandle obj, uint16_t instId);

/**
 * Initialize object.
 * \return 0 Success
 * \return -1 Failure
 */
int32_t GuidanceSettingsInitialize()
{
	// Register object with the object manager
	handle = UAVObjRegister(GUIDANCESETTINGS_OBJID, GUIDANCESETTINGS_NAME, GUIDANCESETTINGS_METANAME, 0,
			GUIDANCESETTINGS_ISSINGLEINST, GUIDANCESETTINGS_ISSETTINGS, GUIDANCESETTINGS_NUMBYTES, &setDefaults);

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
	GuidanceSettingsData data;
	UAVObjMetadata metadata;

	// Initialize object fields to their default values
	UAVObjGetInstanceData(obj, instId, &data);
	memset(&data, 0, sizeof(GuidanceSettingsData));
	data.MaxGroundspeed = 100;
	data.GroundVelocityP = 0.1;
	data.MaxVerticalSpeed = 100;
	data.VertVelocityP = 0.1;
	data.VelP = 0.1;
	data.VelI = 0.1;
	data.VelD = 0;
	data.DownP = 0;
	data.DownI = 0;
	data.DownD = 0;
	data.MaxVelIntegral = 2;
	data.MaxThrottleIntegral = 1;
	data.VelUpdatePeriod = 100;
	data.VelPIDUpdatePeriod = 20;

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
UAVObjHandle GuidanceSettingsHandle()
{
	return handle;
}

/**
 * @}
 */

