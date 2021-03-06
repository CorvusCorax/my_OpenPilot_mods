/**
 ******************************************************************************
 *
 * @file       importexportgadgetoptionspage.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @see        The GNU Public License (GPL) Version 3
 * @brief      Option page for Import/Export Plugin
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup   importexportplugin
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

#include "importexportgadgetoptionspage.h"
#include "importexportgadgetconfiguration.h"
#include "ui_importexportgadgetoptionspage.h"

#include <QFileDialog>
#include <QtDebug>

ImportExportGadgetOptionsPage::ImportExportGadgetOptionsPage(ImportExportGadgetConfiguration *config, QObject *parent) :
        IOptionsPage(parent),
        m_config(config)
{
}

//creates options page widget (uses the UI file)
QWidget *ImportExportGadgetOptionsPage::createPage(QWidget *parent)
{

    options_page = new Ui::ImportExportGadgetOptionsPage();
    //main widget
    QWidget *optionsPageWidget = new QWidget;
    //main layout
    options_page->setupUi(optionsPageWidget);

    // Restore the contents from the settings:
    options_page->iniFile->setExpectedKind(Utils::PathChooser::File);
    options_page->iniFile->setPromptDialogFilter(tr("INI file (*.ini);; XML file (*.xml)"));
    options_page->iniFile->setPromptDialogTitle(tr("Choose configuration file"));
    options_page->iniFile->setPath(m_config->getIniFile());

    return optionsPageWidget;
}

/**
 * Called when the user presses apply or OK.
 *
 * Saves the current values
 *
 */
void ImportExportGadgetOptionsPage::apply()
{
    m_config->setIniFile(options_page->iniFile->path());
}


void ImportExportGadgetOptionsPage::finish()
{

}
/**
 * @}
 * @}
 */
