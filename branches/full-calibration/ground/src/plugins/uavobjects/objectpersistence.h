/**
 ******************************************************************************
 *
 * @file       objectpersistence.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @see        The GNU Public License (GPL) Version 3
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup UAVObjectsPlugin UAVObjects Plugin
 * @{
 *   
 * @note       Object definition file: objectpersistence.xml. 
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
#ifndef OBJECTPERSISTENCE_H
#define OBJECTPERSISTENCE_H

#include "uavdataobject.h"
#include "uavobjectmanager.h"

class UAVOBJECTS_EXPORT ObjectPersistence: public UAVDataObject
{
    Q_OBJECT

public:
    // Field structure
    typedef struct {
        quint8 Operation;
        quint8 Selection;
        quint32 ObjectID;
        quint32 InstanceID;

    } __attribute__((packed)) DataFields;

    // Field information
    // Field Operation information
    /* Enumeration options for field Operation */
    typedef enum { OPERATION_LOAD=0, OPERATION_SAVE=1, OPERATION_DELETE=2 } OperationOptions;
    // Field Selection information
    /* Enumeration options for field Selection */
    typedef enum { SELECTION_SINGLEOBJECT=0, SELECTION_ALLSETTINGS=1, SELECTION_ALLMETAOBJECTS=2, SELECTION_ALLOBJECTS=3 } SelectionOptions;
    // Field ObjectID information
    // Field InstanceID information

  
    // Constants
    static const quint32 OBJID = 572614706U;
    static const QString NAME;
    static const bool ISSINGLEINST = 1;
    static const bool ISSETTINGS = 0;
    static const quint32 NUMBYTES = sizeof(DataFields);

    // Functions
    ObjectPersistence();

    DataFields getData();
    void setData(const DataFields& data);
    Metadata getDefaultMetadata();
    UAVDataObject* clone(quint32 instID);

    static ObjectPersistence* GetInstance(UAVObjectManager* objMngr, quint32 instID = 0);
	
private:
    DataFields data;

    void setDefaultFieldValues();

};

#endif // OBJECTPERSISTENCE_H
