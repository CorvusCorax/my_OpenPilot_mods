/**
 ******************************************************************************
 *
 * @file       gcstelemetrystats.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @see        The GNU Public License (GPL) Version 3
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup UAVObjectsPlugin UAVObjects Plugin
 * @{
 *   
 * @note       Object definition file: gcstelemetrystats.xml. 
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
#ifndef GCSTELEMETRYSTATS_H
#define GCSTELEMETRYSTATS_H

#include "uavdataobject.h"
#include "uavobjectmanager.h"

class UAVOBJECTS_EXPORT GCSTelemetryStats: public UAVDataObject
{
    Q_OBJECT

public:
    // Field structure
    typedef struct {
        quint8 Status;
        float TxDataRate;
        float RxDataRate;
        quint32 TxFailures;
        quint32 RxFailures;
        quint32 TxRetries;

    } __attribute__((packed)) DataFields;

    // Field information
    // Field Status information
    /* Enumeration options for field Status */
    typedef enum { STATUS_DISCONNECTED=0, STATUS_HANDSHAKEREQ=1, STATUS_HANDSHAKEACK=2, STATUS_CONNECTED=3 } StatusOptions;
    // Field TxDataRate information
    // Field RxDataRate information
    // Field TxFailures information
    // Field RxFailures information
    // Field TxRetries information

  
    // Constants
    static const quint32 OBJID = 1998458950U;
    static const QString NAME;
    static const bool ISSINGLEINST = 1;
    static const bool ISSETTINGS = 0;
    static const quint32 NUMBYTES = sizeof(DataFields);

    // Functions
    GCSTelemetryStats();

    DataFields getData();
    void setData(const DataFields& data);
    Metadata getDefaultMetadata();
    UAVDataObject* clone(quint32 instID);

    static GCSTelemetryStats* GetInstance(UAVObjectManager* objMngr, quint32 instID = 0);
	
private:
    DataFields data;

    void setDefaultFieldValues();

};

#endif // GCSTELEMETRYSTATS_H
