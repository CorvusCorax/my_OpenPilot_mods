/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtWeb module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published
 ** by IBM, and/or under the terms of the GNU Lesser General Public License,
 ** version 2.1, as published by the Free Software Foundation.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL and the LGPL along with this
 ** file. See the LICENSE file and the cpl1.0.txt/lgpl-2.1.txt files
 ** included with the source distribution for more information.
 ** If you did not receive a copy of the licenses, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ****************************************************************************/

#ifndef QXTABSTRACTWEBSERVICE_H
#define QXTABSTRACTWEBSERVICE_H

#include <QObject>
#include "qxtabstractwebsessionmanager.h"
class QxtWebEvent;
class QxtWebRequestEvent;

class QxtAbstractWebServicePrivate;
class QXT_WEB_EXPORT QxtAbstractWebService : public QObject
{
    Q_OBJECT
public:
    explicit QxtAbstractWebService(QxtAbstractWebSessionManager* manager, QObject* parent = 0);

    QxtAbstractWebSessionManager* sessionManager() const;
    inline void postEvent(QxtWebEvent* event)
    {
        sessionManager()->postEvent(event);
    }
    virtual void pageRequestedEvent(QxtWebRequestEvent* event) = 0;
    // virtual void functionInvokedEvent(QxtWebRequestEvent* event) = 0; // todo: implement

private:
    QXT_DECLARE_PRIVATE(QxtAbstractWebService)
};

#endif // QXTABSTRACTWEBSERVICE_H
