/**
 ******************************************************************************
 *
 * @file       TCPtelemetryoptionspage.cpp
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

#include "TCPtelemetryoptionspage.h"
#include "TCPtelemetryconfiguration.h"
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include "ui_TCPtelemetryoptionspage.h"


TCPtelemetryOptionsPage::TCPtelemetryOptionsPage(TCPtelemetryConfiguration *config, QObject *parent) :
    IOptionsPage(parent),
    m_config(config)
{
}

QWidget *TCPtelemetryOptionsPage::createPage(QWidget *parent)
{
    m_page = new Ui::TCPtelemetryOptionsPage();
    QWidget *w = new QWidget(parent);
    m_page->setupUi(w);

    m_page->Port->setValue(m_config->Port());
    m_page->HostName->setText(m_config->HostName());

    return w;
}

void TCPtelemetryOptionsPage::apply()
{
    m_config->setPort(m_page->Port->value());
    m_config->setHostName(m_page->HostName->text());
}

void TCPtelemetryOptionsPage::finish()
{
    delete m_page;
}

