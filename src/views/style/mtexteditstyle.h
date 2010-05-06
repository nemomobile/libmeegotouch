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

#ifndef MTEXTEDITSTYLE_H
#define MTEXTEDITSTYLE_H

#include <QFont>
#include <QColor>
#include <QString>
#include <mwidgetstyle.h>

class M_EXPORT MTextEditStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(MTextEditStyle)

    M_STYLE_ATTRIBUTE(QFont,        font,                     Font)
    M_STYLE_ATTRIBUTE(QColor,       textColor,                TextColor)
    M_STYLE_ATTRIBUTE(QColor,       selectionTextColor,       SelectionTextColor)
    M_STYLE_ATTRIBUTE(QColor,       selectionBackgroundColor, SelectionBackgroundColor)
    M_STYLE_ATTRIBUTE(qreal,        selectionThreshold,       SelectionThreshold)
    M_STYLE_ATTRIBUTE(bool,         allowViewReposition,      AllowViewReposition)
    M_STYLE_ATTRIBUTE(QString,      maskString,               MaskString)
    M_STYLE_ATTRIBUTE(QColor,       promptColor,              PromptColor)
    M_STYLE_ATTRIBUTE(QString,      pasteFailedIcon,          PasteFailedIcon)
    M_STYLE_ATTRIBUTE(int,          pasteFailedDuration,      PasteFailedDuration)
    M_STYLE_ATTRIBUTE(int,          maskingDelay,             MaskingDelay)
};

class M_EXPORT MTextEditStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(MTextEditStyle)
};

#endif

