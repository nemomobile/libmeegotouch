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
 
#ifndef MWIDGETRECTANGULARCLIPANIMATION_P_H
#define MWIDGETRECTANGULARCLIPANIMATION_P_H

#include "mabstractwidgetanimation_p.h"
#include "mabstractwidgetanimation.h"

#include <QAbstractAnimation>

class MGraphicsClipEffect;
class MWidgetRectangularClipAnimation;
class QPropertyAnimation;

class MWidgetRectangularClipAnimationPrivate : public MAbstractWidgetAnimationPrivate
{
public:
    MWidgetRectangularClipAnimationPrivate();
    virtual ~MWidgetRectangularClipAnimationPrivate();

    void init();

private:
    Q_DECLARE_PUBLIC(MWidgetRectangularClipAnimation)

    QPropertyAnimation *clipAnimation;
    MGraphicsClipEffect *clipEffect;
    MAbstractWidgetAnimation::TransitionDirection transitionDirection;

#ifdef UNIT_TEST
    friend class Ut_MWidgetRectangularClipAnimation;
#endif
};

#endif // MWIDGETRECTANGULARCLIPANIMATION_P_H
