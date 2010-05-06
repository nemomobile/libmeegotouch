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

#ifndef MWIDGETSLIDEINANIMATION_P_H
#define MWIDGETSLIDEINANIMATION_P_H

#include "mabstractwidgetanimation_p.h"
#include <QPointF>

class QPropertyAnimation;

class MWidgetSlideInAnimationPrivate : public MAbstractWidgetAnimationPrivate
{
    Q_DECLARE_PUBLIC(MWidgetSlideInAnimation)
public:
    virtual ~MWidgetSlideInAnimationPrivate() {}
private:
    QPointF originalPos;
    bool played;
    QPropertyAnimation *positionAnimation;
};

#endif
