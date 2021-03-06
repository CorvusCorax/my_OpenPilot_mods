/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup PositionActual PositionActual 
 * @brief Deprecated for GPS position.
 *
 * Autogenerated files and functions for PositionActual Object
 
 * @{ 
 *
 * @file       positionactual.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the PositionActual object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: positionactual.xml. 
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

#ifndef POSITIONACTUAL_H
#define POSITIONACTUAL_H

// Object constants
#define POSITIONACTUAL_OBJID 3881752126U
#define POSITIONACTUAL_NAME "PositionActual"
#define POSITIONACTUAL_METANAME "PositionActualMeta"
#define POSITIONACTUAL_ISSINGLEINST 1
#define POSITIONACTUAL_ISSETTINGS 0
#define POSITIONACTUAL_NUMBYTES sizeof(PositionActualData)

// Object access macros
/**
 * @function PositionActualGet(dataOut)
 * @brief Populate a PositionActualData object
 * @param[out] dataOut 
 */
#define PositionActualGet(dataOut) UAVObjGetData(PositionActualHandle(), dataOut)
#define PositionActualSet(dataIn) UAVObjSetData(PositionActualHandle(), dataIn)
#define PositionActualInstGet(instId, dataOut) UAVObjGetInstanceData(PositionActualHandle(), instId, dataOut)
#define PositionActualInstSet(instId, dataIn) UAVObjSetInstanceData(PositionActualHandle(), instId, dataIn)
#define PositionActualConnectQueue(queue) UAVObjConnectQueue(PositionActualHandle(), queue, EV_MASK_ALL_UPDATES)
#define PositionActualConnectCallback(cb) UAVObjConnectCallback(PositionActualHandle(), cb, EV_MASK_ALL_UPDATES)
#define PositionActualCreateInstance() UAVObjCreateInstance(PositionActualHandle())
#define PositionActualRequestUpdate() UAVObjRequestUpdate(PositionActualHandle())
#define PositionActualRequestInstUpdate(instId) UAVObjRequestInstanceUpdate(PositionActualHandle(), instId)
#define PositionActualUpdated() UAVObjUpdated(PositionActualHandle())
#define PositionActualInstUpdated(instId) UAVObjUpdated(PositionActualHandle(), instId)
#define PositionActualGetMetadata(dataOut) UAVObjGetMetadata(PositionActualHandle(), dataOut)
#define PositionActualSetMetadata(dataIn) UAVObjSetMetadata(PositionActualHandle(), dataIn)
#define PositionActualReadOnly(dataIn) UAVObjReadOnly(PositionActualHandle())

// Object data
typedef struct {
    uint8_t Status;
    float Latitude;
    float Longitude;
    float Altitude;
    float GeoidSeparation;
    float Heading;
    float Groundspeed;
    float Airspeed;
    float Climbrate;
    int8_t Satellites;
    float PDOP;
    float HDOP;
    float VDOP;
    float NED[3];
    float Vel[3];

} __attribute__((packed)) PositionActualData;

// Field information
// Field Status information
/* Enumeration options for field Status */
typedef enum { POSITIONACTUAL_STATUS_NOGPS=0, POSITIONACTUAL_STATUS_NOFIX=1, POSITIONACTUAL_STATUS_FIX2D=2, POSITIONACTUAL_STATUS_FIX3D=3 } PositionActualStatusOptions;
// Field Latitude information
// Field Longitude information
// Field Altitude information
// Field GeoidSeparation information
// Field Heading information
// Field Groundspeed information
// Field Airspeed information
// Field Climbrate information
// Field Satellites information
// Field PDOP information
// Field HDOP information
// Field VDOP information
// Field NED information
/* Number of elements for field NED */
#define POSITIONACTUAL_NED_NUMELEM 3
// Field Vel information
/* Number of elements for field Vel */
#define POSITIONACTUAL_VEL_NUMELEM 3


// Generic interface functions
int32_t PositionActualInitialize();
UAVObjHandle PositionActualHandle();

#endif // POSITIONACTUAL_H

/**
 * @}
 * @}
 */
