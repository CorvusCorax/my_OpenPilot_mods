/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup ExampleObject2 ExampleObject2 
 * @brief Example object
 *
 * Autogenerated files and functions for ExampleObject2 Object
 
 * @{ 
 *
 * @file       exampleobject2.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the ExampleObject2 object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: exampleobject2.xml. 
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

#ifndef EXAMPLEOBJECT2_H
#define EXAMPLEOBJECT2_H

// Object constants
#define EXAMPLEOBJECT2_OBJID 2743296914U
#define EXAMPLEOBJECT2_NAME "ExampleObject2"
#define EXAMPLEOBJECT2_METANAME "ExampleObject2Meta"
#define EXAMPLEOBJECT2_ISSINGLEINST 0
#define EXAMPLEOBJECT2_ISSETTINGS 0
#define EXAMPLEOBJECT2_NUMBYTES sizeof(ExampleObject2Data)

// Object access macros
/**
 * @function ExampleObject2Get(dataOut)
 * @brief Populate a ExampleObject2Data object
 * @param[out] dataOut 
 */
#define ExampleObject2Get(dataOut) UAVObjGetData(ExampleObject2Handle(), dataOut)
#define ExampleObject2Set(dataIn) UAVObjSetData(ExampleObject2Handle(), dataIn)
#define ExampleObject2InstGet(instId, dataOut) UAVObjGetInstanceData(ExampleObject2Handle(), instId, dataOut)
#define ExampleObject2InstSet(instId, dataIn) UAVObjSetInstanceData(ExampleObject2Handle(), instId, dataIn)
#define ExampleObject2ConnectQueue(queue) UAVObjConnectQueue(ExampleObject2Handle(), queue, EV_MASK_ALL_UPDATES)
#define ExampleObject2ConnectCallback(cb) UAVObjConnectCallback(ExampleObject2Handle(), cb, EV_MASK_ALL_UPDATES)
#define ExampleObject2CreateInstance() UAVObjCreateInstance(ExampleObject2Handle())
#define ExampleObject2RequestUpdate() UAVObjRequestUpdate(ExampleObject2Handle())
#define ExampleObject2RequestInstUpdate(instId) UAVObjRequestInstanceUpdate(ExampleObject2Handle(), instId)
#define ExampleObject2Updated() UAVObjUpdated(ExampleObject2Handle())
#define ExampleObject2InstUpdated(instId) UAVObjUpdated(ExampleObject2Handle(), instId)
#define ExampleObject2GetMetadata(dataOut) UAVObjGetMetadata(ExampleObject2Handle(), dataOut)
#define ExampleObject2SetMetadata(dataIn) UAVObjSetMetadata(ExampleObject2Handle(), dataIn)
#define ExampleObject2ReadOnly(dataIn) UAVObjReadOnly(ExampleObject2Handle())

// Object data
typedef struct {
    int8_t Field1;
    int16_t Field2;
    int32_t Field3;
    float Field4[4];

} __attribute__((packed)) ExampleObject2Data;

// Field information
// Field Field1 information
// Field Field2 information
// Field Field3 information
// Field Field4 information
/* Number of elements for field Field4 */
#define EXAMPLEOBJECT2_FIELD4_NUMELEM 4


// Generic interface functions
int32_t ExampleObject2Initialize();
UAVObjHandle ExampleObject2Handle();

#endif // EXAMPLEOBJECT2_H

/**
 * @}
 * @}
 */
