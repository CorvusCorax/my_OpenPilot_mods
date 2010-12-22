/**
 ******************************************************************************
 *
 * @file       attitudeactual.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @see        The GNU Public License (GPL) Version 3
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup UAVObjectsPlugin UAVObjects Plugin
 * @{
 *   
 * @note       Object definition file: attitudeactual.xml. 
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
#ifndef ATTITUDEACTUAL_H
#define ATTITUDEACTUAL_H

#include "uavdataobject.h"
#include "uavobjectmanager.h"

class UAVOBJECTS_EXPORT AttitudeActual: public UAVDataObject
{
    Q_OBJECT

public:
    // Field structure
    typedef struct {
        float q1;
        float q2;
        float q3;
        float q4;
        float Roll;
        float Pitch;
        float Yaw;

    } __attribute__((packed)) DataFields;

    // Field information
    // Field q1 information
    // Field q2 information
    // Field q3 information
    // Field q4 information
    // Field Roll information
    // Field Pitch information
    // Field Yaw information

  
    // Constants
    static const quint32 OBJID = 4233858292U;
    static const QString NAME;
    static const QString DESCRIPTION;
    static const bool ISSINGLEINST = 1;
    static const bool ISSETTINGS = 0;
    static const quint32 NUMBYTES = sizeof(DataFields);

    // Functions
    AttitudeActual();

    DataFields getData();
    void setData(const DataFields& data);
    Metadata getDefaultMetadata();
    UAVDataObject* clone(quint32 instID);

    static AttitudeActual* GetInstance(UAVObjectManager* objMngr, quint32 instID = 0);
	
private:
    DataFields data;

    void setDefaultFieldValues();

};

#endif // ATTITUDEACTUAL_H
