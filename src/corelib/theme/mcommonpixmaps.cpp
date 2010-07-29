/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mcommonpixmaps.h"
#include "mthemedaemon.h"
#include "mdebug.h"
#include <QFile>
#include <QDir>

using namespace M::MThemeDaemonProtocol;


#define VERSION(major, minor) ((major << 16) | minor)
const unsigned int PRELOAD_FILE_VERSION = VERSION(0, 1);

MCommonPixmaps::MCommonPixmaps(MThemeDaemon *daemon) :
    minRequestsForCache(0),
    daemon(daemon)
{
    connect(&cpuMonitor, SIGNAL(newCpuFrameAvailable()), SLOT(loadOne()));
}

MCommonPixmaps::~MCommonPixmaps()
{
    // please call clear before destroying this object
    Q_ASSERT(mostUsedPixmaps.count() == 0);
}

void MCommonPixmaps::clear()
{
    // release all most used pixmaps
    foreach(const PixmapIdentifier & id, mostUsedPixmaps) {
        if (toLoadList.contains(id))
            continue;

        ImageResource *resource = daemon->findImageResource(id.imageId);
        resource->releasePixmap(id.size);
    }

    cpuMonitor.stop();
    mostUsedPixmaps.clear();
    toLoadList.clear();
    requestCounts.clear();
    minRequestsForCache = 0;
}

bool MCommonPixmaps::load(const QString &filename)
{
    // clear the old ones.
    clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream stream(&file);

    unsigned int version;
    stream >> version;
    if (version != PRELOAD_FILE_VERSION)
        return false;

    while (file.bytesAvailable()) {
        QString imageId;
        QSize size;
        quint32 requestCount;
        bool isMostUsed;
        stream >> imageId >> size >> requestCount >> isMostUsed;

        PixmapIdentifier id(imageId, size);
        requestCounts.insert(id, requestCount);
        if (isMostUsed) {
            toLoadList.insert(PixmapIdentifier(imageId, size));
            mostUsedPixmaps.insert(PixmapIdentifier(imageId, size));
        }
    }

    if (!toLoadList.isEmpty()) {
        cpuMonitor.start(2000);
    }

    file.close();
    return true;
}

bool MCommonPixmaps::save(const QString &filename) const
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream stream(&file);
    stream << PRELOAD_FILE_VERSION;

    QHash<PixmapIdentifier, quint32>::const_iterator i = requestCounts.begin();

    for (; i != requestCounts.end(); ++i) {
        const PixmapIdentifier& id = i.key();

        bool isMostUsed = mostUsedPixmaps.contains(id);
        stream << id.imageId << id.size << i.value() << isMostUsed;
    }

    file.close();
    return true;
}

void MCommonPixmaps::loadOne()
{
    // stop the timer, so we can adjust the frequency depending on the usage
    cpuMonitor.stop();

    if (0 <= cpuMonitor.usage() || cpuMonitor.usage() < 10) {
        //check if there really is something to load
        if (!toLoadList.isEmpty()) {
            PixmapIdentifier id = *toLoadList.begin();
            toLoadList.erase(toLoadList.begin());

            ImageResource *resource = daemon->findImageResource(id.imageId);
            if (resource) {
                resource->fetchPixmap(id.size);
            } else {
                mWarning("MCommonPixmaps") << QString("Themedaemon could not find resource %1 while loading most used pixmaps. Removing from list.").arg(id.imageId);
                requestCounts.remove(id);
                mostUsedPixmaps.remove(id);
            }

            if (!toLoadList.isEmpty()) {
                // there's still items in the list, so start the timer with small delay
                cpuMonitor.start(250);
            } else {
                // all common pixmaps loaded - notify clients
                MostUsedPixmaps mostUsed;
                mostUsed.addedHandles = mostUsedPixmapHandles();
                emit mostUsedPixmapsChanged(mostUsed);
            }
        }
    } else {
        // the cpu usage was too high, so start start the timer with longer delay
        cpuMonitor.start(2000);
    }
}

void MCommonPixmaps::increaseRequestCount(const M::MThemeDaemonProtocol::PixmapIdentifier &id, ImageResource *resource)
{
    QHash<PixmapIdentifier, quint32>::iterator requestCount = requestCounts.find(id);
    if (requestCount == requestCounts.end()) {
        requestCount = requestCounts.insert(id, 0);
    }
    ++requestCount.value();

    // does this pixmap has higher request count value than the current minimum for cache?
    if (requestCount.value() > minRequestsForCache && !mostUsedPixmaps.contains(id)) {

        // this pixmap might end up to mostUsedPixmaps list

        // check if there's still room for this pixmap
        if (mostUsedPixmaps.count() < MCommonPixmaps::CacheSize) {
            // yep, just add this pixmap and return
            Qt::HANDLE handle = resource->fetchPixmap(id.size);
            mostUsedPixmaps.insert(id);

            MostUsedPixmaps packet;
            packet.addedHandles.append(PixmapHandle(id, handle));
            if (toLoadList.isEmpty()) {
                emit mostUsedPixmapsChanged(packet);
            }
            return;
        }

        // there was no room, so we'll check if we can make it
        QSet<PixmapIdentifier>::iterator i = mostUsedPixmaps.begin();
        QSet<PixmapIdentifier>::iterator leastUsed = i;
        quint32 leastUsedRequests = requestCounts[*leastUsed];
        quint32 secondlyLeastUsedRequests = leastUsedRequests;
        ++i;

        // find the least used pixmap from most used list
        for (; i != mostUsedPixmaps.end(); ++i) {
            const PixmapIdentifier &curId = *i;
            quint32 count = requestCounts[curId];
            if (count < leastUsedRequests) {
                secondlyLeastUsedRequests = leastUsedRequests;
                leastUsedRequests = count;
                leastUsed = i;
            }
        }

        // if the least used is still above the current, we'll just update the limit
        if (leastUsedRequests >= requestCount.value()) {
            minRequestsForCache = leastUsedRequests;
            return;
        }

        // otherwise we have a new pixmap for the list

        // update the limit, there may be duplicate request counts in the most used list
        minRequestsForCache = (secondlyLeastUsedRequests > requestCount.value()) ? requestCount.value() : secondlyLeastUsedRequests;

        // allocate one pixmap for the list
        Qt::HANDLE handle = resource->fetchPixmap(id.size);
        MostUsedPixmaps packet;
        packet.addedHandles.append(PixmapHandle(id, handle));

        // release the old one from the list
        if (!toLoadList.remove(*leastUsed)) {
            packet.removedIdentifiers.append(id);
        }
        if (toLoadList.isEmpty()) {
            emit mostUsedPixmapsChanged(packet);
        }

        mostUsedPixmaps.remove(*leastUsed);
        mostUsedPixmaps.insert(id);
    }
}

void MCommonPixmaps::reload(const PixmapIdentifier &id, ImageResource *oldResource)
{
    if (toLoadList.contains(id) || !mostUsedPixmaps.contains(id)) {
        // no need to do anything
        return;
    }

    oldResource->releasePixmap(id.size);
    toLoadList.insert(id);
}

QList<M::MThemeDaemonProtocol::PixmapHandle> MCommonPixmaps::mostUsedPixmapHandles()
{
    if (!toLoadList.isEmpty()) {
        // not all pixmaps loaded/valid - return empty list
        return QList<M::MThemeDaemonProtocol::PixmapHandle>();
    }

    // we could also save the handles earlier but it is cheap to do the query
    QList<PixmapHandle> pixmapHandles;
    foreach(const M::MThemeDaemonProtocol::PixmapIdentifier& id, mostUsedPixmaps) {
        Qt::HANDLE handle = daemon->findImageResource(id.imageId)->pixmapHandle(id.size);
        pixmapHandles.append(M::MThemeDaemonProtocol::PixmapHandle(id, handle));
    }

    return pixmapHandles;
}
