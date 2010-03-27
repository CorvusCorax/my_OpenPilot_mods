/**
 ******************************************************************************
 *
 * @file       mapplugin.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief
 * @see        The GNU Public License (GPL) Version 3
 * @defgroup   map
 * @{
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
#include "mapplugin.h"
#include "mapgadgetfactory.h"
#include <QtPlugin>
#include <QStringList>
#include <extensionsystem/pluginmanager.h>

MapPlugin::MapPlugin()
{
   // Do nothing
}

MapPlugin::~MapPlugin()
{
   // Do nothing
}

bool MapPlugin::initialize(const QStringList& args, QString *errMsg)
{
   Q_UNUSED(args);
   Q_UNUSED(errMsg);
   mf = new MapGadgetFactory(this);
   addAutoReleasedObject(mf);

   return true;
}

void MapPlugin::extensionsInitialized()
{
   // Do nothing
}

void MapPlugin::shutdown()
{
   // Do nothing
}
Q_EXPORT_PLUGIN(MapPlugin)

