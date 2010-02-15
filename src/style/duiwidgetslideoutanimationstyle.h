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

#ifndef DUIWIDGETSLIDEOUTANIMATIONSTYLE_H
#define DUIWIDGETSLIDEOUTANIMATIONSTYLE_H

#include <duiabstractwidgetanimationstyle.h>
#include <QEasingCurve>

//! \internal
class DUI_EXPORT DuiWidgetSlideOutAnimationStyle : public DuiAbstractWidgetAnimationStyle
{
    Q_OBJECT
    DUI_STYLE_INTERNAL(DuiWidgetSlideOutAnimationStyle)

    DUI_STYLE_ATTRIBUTE(QString, to, To)
    DUI_STYLE_ATTRIBUTE(QEasingCurve, easingCurve, EasingCurve)
};

class DUI_EXPORT DuiWidgetSlideOutAnimationStyleContainer : public DuiAbstractWidgetAnimationStyleContainer
{
    DUI_STYLE_CONTAINER_INTERNAL(DuiWidgetSlideOutAnimationStyle)
};
//! \internal_end

#endif
