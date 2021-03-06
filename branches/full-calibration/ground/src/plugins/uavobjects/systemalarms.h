/**
 ******************************************************************************
 *
 * @file       systemalarms.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @see        The GNU Public License (GPL) Version 3
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup UAVObjectsPlugin UAVObjects Plugin
 * @{
 *   
 * @note       Object definition file: systemalarms.xml. 
 *             This is an automatically generated file.
 *             DO NOT modify manually.
 *
 * @brief      The UAVUObjects GCS plugin 
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
#ifndef SYSTEMALARMS_H
#define SYSTEMALARMS_H

#include "uavdataobject.h"
#include "uavobjectmanager.h"

class UAVOBJECTS_EXPORT SystemAlarms: public UAVDataObject
{
    Q_OBJECT

public:
    // Field structure
    typedef struct {
        quint8 Alarm[10];

    } __attribute__((packed)) DataFields;

    // Field information
    // Field Alarm information
    /* Enumeration options for field Alarm */
    typedef enum { ALARM_OK=0, ALARM_WARNING=1, ALARM_ERROR=2, ALARM_CRITICAL=3 } AlarmOptions;
    /* Array element names for field Alarm */
    typedef enum { ALARM_OUTOFMEMORY=0, ALARM_STACKOVERFLOW=1, ALARM_CPUOVERLOAD=2, ALARM_EVENTSYSTEM=3, ALARM_SDCARD=4, ALARM_TELEMETRY=5, ALARM_MANUALCONTROL=6, ALARM_ACTUATOR=7, ALARM_STABILIZATION=8, ALARM_AHRSCOMMS=9 } AlarmElem;
    /* Number of elements for field Alarm */
    static const quint32 ALARM_NUMELEM = 10;

  
    // Constants
    static const quint32 OBJID = 2311311584U;
    static const QString NAME;
    static const bool ISSINGLEINST = 1;
    static const bool ISSETTINGS = 0;
    static const quint32 NUMBYTES = sizeof(DataFields);

    // Functions
    SystemAlarms();

    DataFields getData();
    void setData(const DataFields& data);
    Metadata getDefaultMetadata();
    UAVDataObject* clone(quint32 instID);

    static SystemAlarms* GetInstance(UAVObjectManager* objMngr, quint32 instID = 0);
	
private:
    DataFields data;

    void setDefaultFieldValues();

};

#endif // SYSTEMALARMS_H
