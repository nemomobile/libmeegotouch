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

#ifndef MGRAPHICSCLIPEFFECT_P_H
#define MGRAPHICSCLIPEFFECT_P_H

#include <QObject>
#include <QRect>

class MGraphicsClipEffect;

class MGraphicsClipEffectPrivate
{

#ifdef UNIT_TEST
    friend class Ut_MGraphicsClipEffect;
#endif

public:
    MGraphicsClipEffectPrivate();
    virtual ~MGraphicsClipEffectPrivate();

private:
    Q_DECLARE_PUBLIC(MGraphicsClipEffect)
    MGraphicsClipEffect *q_ptr;

    QRect drawRectangle;
    int margin;
};

#endif // MGRAPHICSCLIPEFFECT_P_H
