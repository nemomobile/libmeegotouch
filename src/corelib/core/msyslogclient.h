/***************************************************************************
**
** Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libmeegotouch.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef MSYSLOGCLIENT_H
#define MSYSLOGCLIENT_H

#include <QUrl>
#include <QMap>
#include <QString>

class QIODevice;

class MSyslogClientSocket
{
    public:
        MSyslogClientSocket();
        virtual ~MSyslogClientSocket();
        bool connectToServer(const QUrl &url);
        void close();

        qint64 sendMsg(QtMsgType type, const char * data);
        qint64 sendMsg(const char * data);

        const QString &syslogId();

    private:

        QMap<QtMsgType, int> severityMap;
        QIODevice * m_socket;
        QString m_syslogId;
};

#endif
