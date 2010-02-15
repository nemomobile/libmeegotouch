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

#ifndef DUIPROGRESSINDICATORBARVIEW_P_H
#define DUIPROGRESSINDICATORBARVIEW_P_H

#include <QObject>
#include "private/duiwidgetview_p.h"

class DuiStyle;
class DuiProgressIndicator;
class QPixmap;
class QTimer;

class DuiProgressIndicatorBarViewPrivate : public DuiWidgetViewPrivate
{
public:
    DuiProgressIndicatorBarViewPrivate();
    virtual ~DuiProgressIndicatorBarViewPrivate();

    DuiProgressIndicator *controller;

    float elementSize;
    int activeElementCount;
    int speed;
    qreal position;
    QTimer *timer;

#ifdef DUI_UNIT_TEST
    DUI_UNIT_TEST;
#endif
};

#endif // DUIPROGRESSINDICATORBARVIEW_P_H
