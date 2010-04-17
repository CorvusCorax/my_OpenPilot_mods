/**
 ******************************************************************************
 *
 * @file       gpsobject.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the GpsObject object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: gpsobject.xml. 
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
#include "gpsobject.h"
#include "uavobjectfields.h"

const QString GpsObject::NAME = QString("GpsObject");

GpsObject::GpsObject(): UAVDataObject(OBJID, ISSINGLEINST, ISSETTINGS, NAME)
{
    // Create fields
    QList<UAVObjectField*> fields;
    fields.append(new UAVObjectFieldFloat(QString("Latitude"), QString("degrees"), 1));
    fields.append(new UAVObjectFieldFloat(QString("Longitude"), QString("degrees"), 1));
    fields.append(new UAVObjectFieldFloat(QString("Altitude"), QString("meters"), 1));

    // Initialize object
    initializeFields(fields, (quint8*)&data, NUMBYTES);
}

UAVObject::Metadata GpsObject::getDefaultMetadata()
{
    UAVObject::Metadata metadata;
    metadata.gcsTelemetryAcked = 1;
    metadata.gcsTelemetryUpdateMode = UAVObject::UPDATEMODE_PERIODIC;
    metadata.gcsTelemetryUpdatePeriod = 1000;
    metadata.flightTelemetryAcked = 1;
    metadata.flightTelemetryUpdateMode = UAVObject::UPDATEMODE_ONCHANGE;
    metadata.flightTelemetryUpdatePeriod = 0;
    metadata.loggingUpdateMode = UAVObject::UPDATEMODE_NEVER;
    metadata.loggingUpdatePeriod = 0;
    return metadata;
}

GpsObject::DataFields GpsObject::getData()
{
    QMutexLocker locker(mutex);
    return data;
}

void GpsObject::setData(DataFields& data)
{
    QMutexLocker locker(mutex);
    this->data = data;
    emit objectUpdatedAuto(this); // trigger object updated event
    emit objectUpdated(this);
}

UAVDataObject* GpsObject::clone(quint32 instID)
{
    GpsObject* obj = new GpsObject();
    obj->initialize(instID, this->getMetaObject());
    return obj;
}
