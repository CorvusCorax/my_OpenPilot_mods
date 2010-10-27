/**
 ******************************************************************************
 * @file
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @see        The GNU Public License (GPL) Version 3
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup importexportplugin
 * @{
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
#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QDialog>
#include "importexportgadgetconfiguration.h"

namespace Ui {
    class ImportExportDialog;
}

class ImportExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportExportDialog( ImportExportGadgetConfiguration *config, QWidget *parent = 0);
    ~ImportExportDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ImportExportDialog *ui;
};

#endif // IMPORTEXPORTDIALOG_H

/**
 * @}
 * @}
 */
