/**
 ******************************************************************************
 *
 * @file       pfdgadgetfactory.cpp
 * @author     Edouard Lafargue Copyright (C) 2010.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup OPMapPlugin Primary Flight Display Plugin
 * @{
 * @brief The Primary Flight Display Gadget 
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
#include "pfdgadgetfactory.h"
#include "pfdgadgetwidget.h"
#include "pfdgadget.h"
#include "pfdgadgetconfiguration.h"
#include "pfdgadgetoptionspage.h"
#include <coreplugin/iuavgadget.h>

PFDGadgetFactory::PFDGadgetFactory(QObject *parent) :
        IUAVGadgetFactory(QString("PFDGadget"),
                          tr("Primary Flight Display Gadget"),
                          parent)
{
}

PFDGadgetFactory::~PFDGadgetFactory()
{
}

Core::IUAVGadget* PFDGadgetFactory::createGadget(QWidget *parent)
{
    PFDGadgetWidget* gadgetWidget = new PFDGadgetWidget(parent);
    return new PFDGadget(QString("PFDGadget"), gadgetWidget, parent);
}

IUAVGadgetConfiguration *PFDGadgetFactory::createConfiguration(QSettings* qSettings)
{
    return new PFDGadgetConfiguration(QString("PFDGadget"), qSettings);
}

IOptionsPage *PFDGadgetFactory::createOptionsPage(IUAVGadgetConfiguration *config)
{
    return new PFDGadgetOptionsPage(qobject_cast<PFDGadgetConfiguration*>(config));
}

