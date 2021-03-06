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

#ifndef APSCANNER_H
#define APSCANNER_H

#include <QObject>
#include <QStringList>

class QTimer;

class APScanner: public QObject
{
    Q_OBJECT

public:
    struct AccessPointInfo {
        int signalStrength;
        QString stationName;
        QString encryption;
    };
    APScanner();
    ~APScanner();

public:
    void startScanning();

public Q_SLOTS:
    void updateAccessPoints();

Q_SIGNALS:
    void accessPointUpdate(const APScanner::AccessPointInfo &accessPoint);

private:
    QStringList accessPointNames;
    QList<AccessPointInfo> accessPointInfos;
    QTimer *timer;
};

#endif
