/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup ManualControlSettings ManualControlSettings 
 * @brief Settings to indicate how to decode receiver input by @ref ManualControlModule.
 *
 * Autogenerated files and functions for ManualControlSettings Object
 
 * @{ 
 *
 * @file       manualcontrolsettings.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the ManualControlSettings object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: manualcontrolsettings.xml. 
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

#ifndef MANUALCONTROLSETTINGS_H
#define MANUALCONTROLSETTINGS_H

// Object constants
#define MANUALCONTROLSETTINGS_OBJID 1845767664U
#define MANUALCONTROLSETTINGS_NAME "ManualControlSettings"
#define MANUALCONTROLSETTINGS_METANAME "ManualControlSettingsMeta"
#define MANUALCONTROLSETTINGS_ISSINGLEINST 1
#define MANUALCONTROLSETTINGS_ISSETTINGS 1
#define MANUALCONTROLSETTINGS_NUMBYTES sizeof(ManualControlSettingsData)

// Object access macros
/**
 * @function ManualControlSettingsGet(dataOut)
 * @brief Populate a ManualControlSettingsData object
 * @param[out] dataOut 
 */
#define ManualControlSettingsGet(dataOut) UAVObjGetData(ManualControlSettingsHandle(), dataOut)
#define ManualControlSettingsSet(dataIn) UAVObjSetData(ManualControlSettingsHandle(), dataIn)
#define ManualControlSettingsInstGet(instId, dataOut) UAVObjGetInstanceData(ManualControlSettingsHandle(), instId, dataOut)
#define ManualControlSettingsInstSet(instId, dataIn) UAVObjSetInstanceData(ManualControlSettingsHandle(), instId, dataIn)
#define ManualControlSettingsConnectQueue(queue) UAVObjConnectQueue(ManualControlSettingsHandle(), queue, EV_MASK_ALL_UPDATES)
#define ManualControlSettingsConnectCallback(cb) UAVObjConnectCallback(ManualControlSettingsHandle(), cb, EV_MASK_ALL_UPDATES)
#define ManualControlSettingsCreateInstance() UAVObjCreateInstance(ManualControlSettingsHandle())
#define ManualControlSettingsRequestUpdate() UAVObjRequestUpdate(ManualControlSettingsHandle())
#define ManualControlSettingsRequestInstUpdate(instId) UAVObjRequestInstanceUpdate(ManualControlSettingsHandle(), instId)
#define ManualControlSettingsUpdated() UAVObjUpdated(ManualControlSettingsHandle())
#define ManualControlSettingsInstUpdated(instId) UAVObjUpdated(ManualControlSettingsHandle(), instId)
#define ManualControlSettingsGetMetadata(dataOut) UAVObjGetMetadata(ManualControlSettingsHandle(), dataOut)
#define ManualControlSettingsSetMetadata(dataIn) UAVObjSetMetadata(ManualControlSettingsHandle(), dataIn)
#define ManualControlSettingsReadOnly(dataIn) UAVObjReadOnly(ManualControlSettingsHandle())

// Object data
typedef struct {
    uint8_t InputMode;
    uint8_t Roll;
    uint8_t Pitch;
    uint8_t Yaw;
    uint8_t Throttle;
    uint8_t FlightMode;
    uint8_t Accessory1;
    uint8_t Accessory2;
    uint8_t Accessory3;
    int16_t ChannelMax[8];
    int16_t ChannelNeutral[8];
    int16_t ChannelMin[8];

} __attribute__((packed)) ManualControlSettingsData;

// Field information
// Field InputMode information
/* Enumeration options for field InputMode */
typedef enum { MANUALCONTROLSETTINGS_INPUTMODE_PWM=0, MANUALCONTROLSETTINGS_INPUTMODE_PPM=1, MANUALCONTROLSETTINGS_INPUTMODE_SPEKTRUM=2 } ManualControlSettingsInputModeOptions;
// Field Roll information
/* Enumeration options for field Roll */
typedef enum { MANUALCONTROLSETTINGS_ROLL_CHANNEL0=0, MANUALCONTROLSETTINGS_ROLL_CHANNEL1=1, MANUALCONTROLSETTINGS_ROLL_CHANNEL2=2, MANUALCONTROLSETTINGS_ROLL_CHANNEL3=3, MANUALCONTROLSETTINGS_ROLL_CHANNEL4=4, MANUALCONTROLSETTINGS_ROLL_CHANNEL5=5, MANUALCONTROLSETTINGS_ROLL_CHANNEL6=6, MANUALCONTROLSETTINGS_ROLL_CHANNEL7=7, MANUALCONTROLSETTINGS_ROLL_NONE=8 } ManualControlSettingsRollOptions;
// Field Pitch information
/* Enumeration options for field Pitch */
typedef enum { MANUALCONTROLSETTINGS_PITCH_CHANNEL0=0, MANUALCONTROLSETTINGS_PITCH_CHANNEL1=1, MANUALCONTROLSETTINGS_PITCH_CHANNEL2=2, MANUALCONTROLSETTINGS_PITCH_CHANNEL3=3, MANUALCONTROLSETTINGS_PITCH_CHANNEL4=4, MANUALCONTROLSETTINGS_PITCH_CHANNEL5=5, MANUALCONTROLSETTINGS_PITCH_CHANNEL6=6, MANUALCONTROLSETTINGS_PITCH_CHANNEL7=7, MANUALCONTROLSETTINGS_PITCH_NONE=8 } ManualControlSettingsPitchOptions;
// Field Yaw information
/* Enumeration options for field Yaw */
typedef enum { MANUALCONTROLSETTINGS_YAW_CHANNEL0=0, MANUALCONTROLSETTINGS_YAW_CHANNEL1=1, MANUALCONTROLSETTINGS_YAW_CHANNEL2=2, MANUALCONTROLSETTINGS_YAW_CHANNEL3=3, MANUALCONTROLSETTINGS_YAW_CHANNEL4=4, MANUALCONTROLSETTINGS_YAW_CHANNEL5=5, MANUALCONTROLSETTINGS_YAW_CHANNEL6=6, MANUALCONTROLSETTINGS_YAW_CHANNEL7=7, MANUALCONTROLSETTINGS_YAW_NONE=8 } ManualControlSettingsYawOptions;
// Field Throttle information
/* Enumeration options for field Throttle */
typedef enum { MANUALCONTROLSETTINGS_THROTTLE_CHANNEL0=0, MANUALCONTROLSETTINGS_THROTTLE_CHANNEL1=1, MANUALCONTROLSETTINGS_THROTTLE_CHANNEL2=2, MANUALCONTROLSETTINGS_THROTTLE_CHANNEL3=3, MANUALCONTROLSETTINGS_THROTTLE_CHANNEL4=4, MANUALCONTROLSETTINGS_THROTTLE_CHANNEL5=5, MANUALCONTROLSETTINGS_THROTTLE_CHANNEL6=6, MANUALCONTROLSETTINGS_THROTTLE_CHANNEL7=7, MANUALCONTROLSETTINGS_THROTTLE_NONE=8 } ManualControlSettingsThrottleOptions;
// Field FlightMode information
/* Enumeration options for field FlightMode */
typedef enum { MANUALCONTROLSETTINGS_FLIGHTMODE_CHANNEL0=0, MANUALCONTROLSETTINGS_FLIGHTMODE_CHANNEL1=1, MANUALCONTROLSETTINGS_FLIGHTMODE_CHANNEL2=2, MANUALCONTROLSETTINGS_FLIGHTMODE_CHANNEL3=3, MANUALCONTROLSETTINGS_FLIGHTMODE_CHANNEL4=4, MANUALCONTROLSETTINGS_FLIGHTMODE_CHANNEL5=5, MANUALCONTROLSETTINGS_FLIGHTMODE_CHANNEL6=6, MANUALCONTROLSETTINGS_FLIGHTMODE_CHANNEL7=7, MANUALCONTROLSETTINGS_FLIGHTMODE_NONE=8 } ManualControlSettingsFlightModeOptions;
// Field Accessory1 information
/* Enumeration options for field Accessory1 */
typedef enum { MANUALCONTROLSETTINGS_ACCESSORY1_CHANNEL0=0, MANUALCONTROLSETTINGS_ACCESSORY1_CHANNEL1=1, MANUALCONTROLSETTINGS_ACCESSORY1_CHANNEL2=2, MANUALCONTROLSETTINGS_ACCESSORY1_CHANNEL3=3, MANUALCONTROLSETTINGS_ACCESSORY1_CHANNEL4=4, MANUALCONTROLSETTINGS_ACCESSORY1_CHANNEL5=5, MANUALCONTROLSETTINGS_ACCESSORY1_CHANNEL6=6, MANUALCONTROLSETTINGS_ACCESSORY1_CHANNEL7=7, MANUALCONTROLSETTINGS_ACCESSORY1_NONE=8 } ManualControlSettingsAccessory1Options;
// Field Accessory2 information
/* Enumeration options for field Accessory2 */
typedef enum { MANUALCONTROLSETTINGS_ACCESSORY2_CHANNEL0=0, MANUALCONTROLSETTINGS_ACCESSORY2_CHANNEL1=1, MANUALCONTROLSETTINGS_ACCESSORY2_CHANNEL2=2, MANUALCONTROLSETTINGS_ACCESSORY2_CHANNEL3=3, MANUALCONTROLSETTINGS_ACCESSORY2_CHANNEL4=4, MANUALCONTROLSETTINGS_ACCESSORY2_CHANNEL5=5, MANUALCONTROLSETTINGS_ACCESSORY2_CHANNEL6=6, MANUALCONTROLSETTINGS_ACCESSORY2_CHANNEL7=7, MANUALCONTROLSETTINGS_ACCESSORY2_NONE=8 } ManualControlSettingsAccessory2Options;
// Field Accessory3 information
/* Enumeration options for field Accessory3 */
typedef enum { MANUALCONTROLSETTINGS_ACCESSORY3_CHANNEL0=0, MANUALCONTROLSETTINGS_ACCESSORY3_CHANNEL1=1, MANUALCONTROLSETTINGS_ACCESSORY3_CHANNEL2=2, MANUALCONTROLSETTINGS_ACCESSORY3_CHANNEL3=3, MANUALCONTROLSETTINGS_ACCESSORY3_CHANNEL4=4, MANUALCONTROLSETTINGS_ACCESSORY3_CHANNEL5=5, MANUALCONTROLSETTINGS_ACCESSORY3_CHANNEL6=6, MANUALCONTROLSETTINGS_ACCESSORY3_CHANNEL7=7, MANUALCONTROLSETTINGS_ACCESSORY3_NONE=8 } ManualControlSettingsAccessory3Options;
// Field ChannelMax information
/* Number of elements for field ChannelMax */
#define MANUALCONTROLSETTINGS_CHANNELMAX_NUMELEM 8
// Field ChannelNeutral information
/* Number of elements for field ChannelNeutral */
#define MANUALCONTROLSETTINGS_CHANNELNEUTRAL_NUMELEM 8
// Field ChannelMin information
/* Number of elements for field ChannelMin */
#define MANUALCONTROLSETTINGS_CHANNELMIN_NUMELEM 8


// Generic interface functions
int32_t ManualControlSettingsInitialize();
UAVObjHandle ManualControlSettingsHandle();

#endif // MANUALCONTROLSETTINGS_H

/**
 * @}
 * @}
 */
