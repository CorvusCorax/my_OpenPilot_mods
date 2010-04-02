/**
 ******************************************************************************
 *
 * @file       examplesettings.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the ExampleSettings object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: examplesettings.xml. 
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
#ifndef EXAMPLESETTINGS_H
#define EXAMPLESETTINGS_H

#include "uavdataobject.h"
#include "uavobjectfieldprimitives.h"

class ExampleSettings: public UAVDataObject
{
public:
    // Field structure
    typedef struct {
        qint32 UpdatePeriod;
        qint32 StepSize;
        quint8 StepDirection;

    } __attribute__((packed)) DataFields;

    // Enumeration types
    typedef enum { STEPDIRECTION_UP=0, STEPDIRECTION_DOWN=1,  } STEPDIRECTIONEnum;

  
    // Constants
    static const quint32 OBJID = 1640607828U;
    static const QString NAME;
    static const bool ISSINGLEINST = 1;
    static const bool ISSETTINGS = 1;
    static const quint32 NUMBYTES = sizeof(DataFields);

    // Functions
    ExampleSettings();

    DataFields getData();
    void setData(DataFields& data);
    Metadata getDefaultMetadata();
    UAVDataObject* clone(quint32 instID);
	
private:
    DataFields data;

};

#endif // EXAMPLESETTINGS_H
