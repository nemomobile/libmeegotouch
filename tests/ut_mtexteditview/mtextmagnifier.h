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
#ifndef MTEXTMAGNIFIER_H
#define MTEXTMAGNIFIER_H

#include <QRectF>

class MWidget;
class QSizeF;
class QPointF;

// Stub text magnifier
class MTextMagnifier {
public:
    enum DeletionPolicy {
        KeepWhenDone,
        DestroyWhenDone
    };
    MTextMagnifier(const MWidget &, const QSizeF &)
    {
    }
    void appear()
    {
    }
    void disappear(DeletionPolicy)
    {
    }
    void setMagnifiedPosition(const QPointF &)
    {
    }
    bool isAppeared() const
    {
        return false;
    }

    QRectF sceneBoundingRect() const
    {
        return QRectF();
    }
};

#endif

