/**
 ******************************************************************************
 *
 * @file       ahrssettings.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @see        The GNU Public License (GPL) Version 3
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup UAVObjectsPlugin UAVObjects Plugin
 * @{
 *   
 * @note       Object definition file: ahrssettings.xml. 
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
#ifndef AHRSSETTINGS_H
#define AHRSSETTINGS_H

#include "uavdataobject.h"
#include "uavobjectmanager.h"

class UAVOBJECTS_EXPORT AHRSSettings: public UAVDataObject
{
    Q_OBJECT

public:
    // Field structure
    typedef struct {
        quint8 Algorithm;
        quint8 Downsampling;
        quint8 UpdatePeriod;
        float IndoorVelocityVariance;
        float YawBias;
        float PitchBias;
        float RollBias;

    } __attribute__((packed)) DataFields;

    // Field information
    // Field Algorithm information
    /* Enumeration options for field Algorithm */
    typedef enum { ALGORITHM_SIMPLE=0, ALGORITHM_INSGPS_INDOOR_NOMAG=1, ALGORITHM_INSGPS_INDOOR=2, ALGORITHM_INSGPS_OUTDOOR=3 } AlgorithmOptions;
    // Field Downsampling information
    // Field UpdatePeriod information
    // Field IndoorVelocityVariance information
    // Field YawBias information
    // Field PitchBias information
    // Field RollBias information

  
    // Constants
    static const quint32 OBJID = 3427867870U;
    static const QString NAME;
    static const bool ISSINGLEINST = 1;
    static const bool ISSETTINGS = 1;
    static const quint32 NUMBYTES = sizeof(DataFields);

    // Functions
    AHRSSettings();

    DataFields getData();
    void setData(const DataFields& data);
    Metadata getDefaultMetadata();
    UAVDataObject* clone(quint32 instID);

    static AHRSSettings* GetInstance(UAVObjectManager* objMngr, quint32 instID = 0);
	
private:
    DataFields data;

    void setDefaultFieldValues();

};

#endif // AHRSSETTINGS_H
