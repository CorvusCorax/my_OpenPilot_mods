/**
 ******************************************************************************
 *
 * @file       rawhidplugin.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup RawHIDPlugin Raw HID Plugin
 * @{
 * @brief Impliments a HID USB connection to the flight hardware as a QIODevice
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

#include "rawhidplugin.h"
#include "rawhid.h"
#include <extensionsystem/pluginmanager.h>

#include <QtCore/QtPlugin>
#include <QtCore/QMutexLocker>

#include "pjrc_rawhid.h"

#include "rawhid_const.h"


RawHIDEnumerationThread::RawHIDEnumerationThread(RawHIDConnection *rawhid)
    : m_rawhid(rawhid),
    m_running(true)
{
}

RawHIDEnumerationThread::~RawHIDEnumerationThread()
{
    m_running = false;
    //wait for the thread to terminate
    if(wait(1000) == false)
        qDebug() << "Cannot terminate RawHIDEnumerationThread";
}

void RawHIDEnumerationThread::run()
{
    QStringList devices = m_rawhid->availableDevices();

    while(m_running)
    {
        if(!m_rawhid->deviceOpened())
        {
            QStringList newDev = m_rawhid->availableDevices();
            if(devices != newDev)
            {
                devices = newDev;
                emit enumerationChanged();
            }
        }

        msleep(500); //update available devices twice per second (doesn't need more)
    }
}


RawHIDConnection::RawHIDConnection()
    : m_enumerateThread(this)
{
    //added by andrew
    RawHidHandle = NULL;


    QObject::connect(&m_enumerateThread, SIGNAL(enumerationChanged()),
                     this, SLOT(onEnumerationChanged()));
    m_enumerateThread.start();
}

RawHIDConnection::~RawHIDConnection()
{}

void RawHIDConnection::onEnumerationChanged()
{
    emit availableDevChanged(this);
}

QStringList RawHIDConnection::availableDevices()
{
    QMutexLocker locker(&m_enumMutex);

    QStringList devices;
    pjrc_rawhid dev;

    //open all device we can
    int opened = dev.open(MAX_DEVICES, VID, PID, USAGE_PAGE, USAGE);

    //for each devices found, get serial number and close it back
    for(int i=0; i<opened; i++)
    {
        devices.append(dev.getserial(i));
        dev.close(i);
    }

    return devices;
}

QIODevice *RawHIDConnection::openDevice(const QString &deviceName)
{
    //added by andrew
    if (RawHidHandle){
        closeDevice(deviceName);
    }
    //end added by andrew
    m_deviceOpened = true;
    //return new RawHID(deviceName);
    RawHidHandle = new RawHID(deviceName);
    return RawHidHandle;
}

void RawHIDConnection::closeDevice(const QString &deviceName)
{
    //added by andrew...
    if (RawHidHandle){
        RawHidHandle->close();
        delete(RawHidHandle);
        RawHidHandle=NULL;
    }
    //end added by andrew

    m_deviceOpened = false;
}


QString RawHIDConnection::connectionName()
{
    return QString("Raw HID USB");
}

QString RawHIDConnection::shortName()
{
    return QString("USB");
}




//usb hid test thread
//temporary...
RawHIDTestThread::RawHIDTestThread()
{
    Core::ConnectionManager *cm = Core::ICore::instance()->connectionManager();
    QObject::connect(cm, SIGNAL(deviceConnected(QIODevice *)),
                     this, SLOT(onDeviceConnect(QIODevice *)));
    QObject::connect(cm, SIGNAL(deviceDisconnected()),
                     this, SLOT(onDeviceDisconnect()));
}

void RawHIDTestThread::onDeviceConnect(QIODevice *dev)
{
    this->dev = dev;
    dev->open(QIODevice::ReadWrite);
    QObject::connect(dev, SIGNAL(readyRead()),
                     this, SLOT(onReadyRead()));

    QObject::connect(dev, SIGNAL(bytesWritten(qint64)),
                     this, SLOT(onBytesWritten(qint64)));
    dev->write("Hello raw hid device\n");
}

void RawHIDTestThread::onDeviceDisconnect()
{
    dev->close();
}

void RawHIDTestThread::onReadyRead()
{
    qDebug() << "Rx:" << dev->readLine(32);
}

void RawHIDTestThread::onBytesWritten(qint64 sz)
{
    qDebug() << "Sent " << sz << " bytes";
}

RawHIDPlugin::RawHIDPlugin()
{
}

RawHIDPlugin::~RawHIDPlugin()
{

}

void RawHIDPlugin::extensionsInitialized()
{
    addAutoReleasedObject(new RawHIDConnection);

    //temp for test
    //addAutoReleasedObject(new RawHIDTestThread);
}

bool RawHIDPlugin::initialize(const QStringList & arguments, QString * errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}


Q_EXPORT_PLUGIN(RawHIDPlugin)
