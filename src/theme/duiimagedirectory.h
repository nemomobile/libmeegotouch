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

#ifndef DUIIMAGEDIRECTORY_H
#define DUIIMAGEDIRECTORY_H

#include <QString>
#include <QSharedPointer>
#include <QSvgRenderer>
#include <QHash>
#include <QSet>
#include <QSize>
#include <QImage>
#include <QColor>
#include <duinamespace.h>

class QPixmap;

uint qHash(const QSize &size);

//! \internal
struct PixmapCacheEntry {
    PixmapCacheEntry() : pixmap(NULL), refCount(0) {}
    ~PixmapCacheEntry() {
        Q_ASSERT(refCount == 0);
    }
    QPixmap *pixmap;
    unsigned int refCount;
};

class ImageResource
{
public:
    virtual ~ImageResource() {}
    // creates a cache entry if there's no such yet, otherwise just increase the refCount
    Qt::HANDLE fetchPixmap(const QSize &size);
    // decreases refCount and releases the pixmap if refCount dropped to zero
    void releasePixmap(const QSize &size);

    // returns a handle to pixmap, without increasing the refCount
    Qt::HANDLE pixmapHandle(const QSize &size);
protected:
    virtual QPixmap *createPixmap(const QSize &size) = 0;
private:
    // pixmaps created from this image resource
    QHash<QSize, PixmapCacheEntry> cachedPixmaps;
};

class IconImageResource : public ImageResource
{
public:
    IconImageResource(const QString &absoluteFilePath) :
        absoluteFilePath(absoluteFilePath), renderer(NULL) {}
    virtual ~IconImageResource() {
        delete renderer;
    }

protected:
    virtual QPixmap *createPixmap(const QSize &size);
private:
    QString absoluteFilePath;
    QSvgRenderer *renderer;
};

class PixmapImageResource : public ImageResource
{
public:
    PixmapImageResource(const QString &absoluteFilePath) :
        absoluteFilePath(absoluteFilePath), image(NULL) {}
    virtual ~PixmapImageResource() {
        delete image;
    }

protected:
    virtual QPixmap *createPixmap(const QSize &size);
private:
    QString absoluteFilePath;
    QImage *image;
};

class SvgImageResource : public ImageResource
{
public:
    SvgImageResource(const QString &imageId, const QSharedPointer<QSvgRenderer>& renderer) :
        imageId(imageId), renderer(renderer) {}
    virtual ~SvgImageResource() {}

protected:
    virtual QPixmap *createPixmap(const QSize &size);
private:
    QString imageId;
    QSharedPointer<QSvgRenderer> renderer;
};


class DuiThemeImagesDirectory
{
public:
    static const QString pixmapsDir;
    static const QString iconsDir;
    static const QString svgDir;

    DuiThemeImagesDirectory(const QString &path, const QString &locale = QString());
    ~DuiThemeImagesDirectory();

    ImageResource *findImage(const QString &imageId);

    bool isLocaleSpecificImage(const QString &imageId) const;

    void reloadLocaleSpecificImages(const QString &locale);
private:
    // image id => image resource
    QHash<QString, ImageResource *> imageIds;

    // locale specific icon
    QHash<QString, ImageResource *> localeSpecificIcons;

    // svg renderer file path => shared svg renderer pointer
    QHash< QString, QSharedPointer<QSvgRenderer> > svgFiles;

    QSet<QString> notFoundIds;

    QString path;
};


class DuiImageDirectory
{
public:

    DuiImageDirectory(const QString &path, Dui::RecursionMode recursionMode);

    ~DuiImageDirectory();

    ImageResource *findImage(const QString &imageId);
private:

    // image id => image resource
    QHash<QString, ImageResource *> imageIds;

    // svg renderer file path => shared svg renderer pointer
    QHash< QString, QSharedPointer<QSvgRenderer> > svgFiles;

    QSet<QString> notFoundIds;
};
//! \internal_end

#endif
