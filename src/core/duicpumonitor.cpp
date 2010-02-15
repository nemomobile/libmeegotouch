/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libdui.
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

#include <QTimer>
#include <QFile>
#include <QString>
#include <QStringList>

#include "duicpumonitor.h"


class DuiCpuMonitorPrivate
{
public:
    QTimer timer;

    bool usable;
    int lastFrameUsage;

#ifdef __linux__
    unsigned long jiffies_total;
    unsigned long jiffies_idle;

    void getCpuUsageInformation(unsigned long &total, unsigned long &idle);
#endif
};


DuiCpuMonitor::DuiCpuMonitor() : d_ptr(new DuiCpuMonitorPrivate)
{
    Q_D(DuiCpuMonitor);

    d->usable = false;

#ifdef __linux__
    d->getCpuUsageInformation(d->jiffies_total, d->jiffies_idle);
#endif

    connect(&d->timer, SIGNAL(timeout()), SLOT(timedOut()));
}

DuiCpuMonitor::~DuiCpuMonitor()
{
    Q_D(DuiCpuMonitor);

    d->timer.stop();

    delete d_ptr;
}


int DuiCpuMonitor::usage()
{
    Q_D(DuiCpuMonitor);

    if (!d->usable)
        return -1;

    return d->lastFrameUsage;
}


void DuiCpuMonitor::start(unsigned msec)
{
    Q_D(DuiCpuMonitor);

    d->timer.start(msec);
}

void DuiCpuMonitor::stop()
{
    Q_D(DuiCpuMonitor);

    d->timer.stop();
}


void DuiCpuMonitor::timedOut()
{
#ifdef __linux__
    Q_D(DuiCpuMonitor);

    unsigned long n_tot = 0;
    unsigned long n_idle = 0;

    d->getCpuUsageInformation(n_tot, n_idle);

    if (n_tot - d->jiffies_total) {
        d->lastFrameUsage = 100 - 100 * (n_idle - d->jiffies_idle) / (n_tot - d->jiffies_total);
        d->usable = true;

        emit newCpuFrameAvailable();

        d->jiffies_idle = n_idle;
        d->jiffies_total = n_tot;
    }
#endif
}


#ifdef __linux__
void DuiCpuMonitorPrivate::getCpuUsageInformation(unsigned long &total, unsigned long &idle)
{
    QFile statFile("/proc/stat");

    if (statFile.exists() && statFile.open(QIODevice::ReadOnly)) {
        char buf[256];
        QStringList l;
        unsigned long sum = 0;
        unsigned long tmp;

        statFile.readLine(buf, 256);
        l = QString::fromAscii(buf).split(' ');

        int idx = 0;
        QStringList::const_iterator end = l.constEnd();
        for (QStringList::const_iterator i = l.constBegin(); i != end; ++i) {
            if (!i->isEmpty()) {
                if (idx) {
                    bool success = true;

                    tmp = i->toULong(&success);
                    if (success) {
                        sum += tmp;

                        if (idx == 4)
                            idle = tmp;
                    }
                }

                idx++;
            }
        }

        total = sum;
    }
}
#endif

