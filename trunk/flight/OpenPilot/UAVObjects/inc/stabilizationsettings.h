/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup StabilizationSettings StabilizationSettings 
 * @brief PID settings used by the Stabilization module to combine the @ref AttitudeActual and @ref AttitudeDesired to compute @ref ActuatorDesired
 *
 * Autogenerated files and functions for StabilizationSettings Object
 
 * @{ 
 *
 * @file       stabilizationsettings.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the StabilizationSettings object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: stabilizationsettings.xml. 
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

#ifndef STABILIZATIONSETTINGS_H
#define STABILIZATIONSETTINGS_H

// Object constants
#define STABILIZATIONSETTINGS_OBJID 117768092U
#define STABILIZATIONSETTINGS_NAME "StabilizationSettings"
#define STABILIZATIONSETTINGS_METANAME "StabilizationSettingsMeta"
#define STABILIZATIONSETTINGS_ISSINGLEINST 1
#define STABILIZATIONSETTINGS_ISSETTINGS 1
#define STABILIZATIONSETTINGS_NUMBYTES sizeof(StabilizationSettingsData)

// Object access macros
/**
 * @function StabilizationSettingsGet(dataOut)
 * @brief Populate a StabilizationSettingsData object
 * @param[out] dataOut 
 */
#define StabilizationSettingsGet(dataOut) UAVObjGetData(StabilizationSettingsHandle(), dataOut)
#define StabilizationSettingsSet(dataIn) UAVObjSetData(StabilizationSettingsHandle(), dataIn)
#define StabilizationSettingsInstGet(instId, dataOut) UAVObjGetInstanceData(StabilizationSettingsHandle(), instId, dataOut)
#define StabilizationSettingsInstSet(instId, dataIn) UAVObjSetInstanceData(StabilizationSettingsHandle(), instId, dataIn)
#define StabilizationSettingsConnectQueue(queue) UAVObjConnectQueue(StabilizationSettingsHandle(), queue, EV_MASK_ALL_UPDATES)
#define StabilizationSettingsConnectCallback(cb) UAVObjConnectCallback(StabilizationSettingsHandle(), cb, EV_MASK_ALL_UPDATES)
#define StabilizationSettingsCreateInstance() UAVObjCreateInstance(StabilizationSettingsHandle())
#define StabilizationSettingsRequestUpdate() UAVObjRequestUpdate(StabilizationSettingsHandle())
#define StabilizationSettingsRequestInstUpdate(instId) UAVObjRequestInstanceUpdate(StabilizationSettingsHandle(), instId)
#define StabilizationSettingsUpdated() UAVObjUpdated(StabilizationSettingsHandle())
#define StabilizationSettingsInstUpdated(instId) UAVObjUpdated(StabilizationSettingsHandle(), instId)
#define StabilizationSettingsGetMetadata(dataOut) UAVObjGetMetadata(StabilizationSettingsHandle(), dataOut)
#define StabilizationSettingsSetMetadata(dataIn) UAVObjSetMetadata(StabilizationSettingsHandle(), dataIn)
#define StabilizationSettingsReadOnly(dataIn) UAVObjReadOnly(StabilizationSettingsHandle())

// Object data
typedef struct {
    uint8_t UpdatePeriod;
    uint8_t RollMax;
    uint8_t PitchMax;
    uint8_t YawMax;
    uint8_t YawMode;
    float ThrottleMax;
    float ThrottleMin;
    float RollIntegralLimit;
    float PitchIntegralLimit;
    float YawIntegralLimit;
    float PitchKp;
    float PitchKi;
    float PitchKd;
    float RollKp;
    float RollKi;
    float RollKd;
    float YawKp;
    float YawKi;
    float YawKd;

} __attribute__((packed)) StabilizationSettingsData;

// Field information
// Field UpdatePeriod information
// Field RollMax information
// Field PitchMax information
// Field YawMax information
// Field YawMode information
/* Enumeration options for field YawMode */
typedef enum { STABILIZATIONSETTINGS_YAWMODE_RATE=0, STABILIZATIONSETTINGS_YAWMODE_HEADING=1 } StabilizationSettingsYawModeOptions;
// Field ThrottleMax information
// Field ThrottleMin information
// Field RollIntegralLimit information
// Field PitchIntegralLimit information
// Field YawIntegralLimit information
// Field PitchKp information
// Field PitchKi information
// Field PitchKd information
// Field RollKp information
// Field RollKi information
// Field RollKd information
// Field YawKp information
// Field YawKi information
// Field YawKd information


// Generic interface functions
int32_t StabilizationSettingsInitialize();
UAVObjHandle StabilizationSettingsHandle();

#endif // STABILIZATIONSETTINGS_H

/**
 * @}
 * @}
 */
