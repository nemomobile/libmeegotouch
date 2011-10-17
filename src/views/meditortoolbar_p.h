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
#ifndef MEDITORTOOLBAR_P_H
#define MEDITORTOOLBAR_P_H

#include "meditortoolbararrow.h"

#include <QList>
#include <QObject>
#include <QPointer>
#include <QPropertyAnimation>
#include <QSizeF>
#include <QTimer>

class EatMButtonGestureFilter;
class MButton;
class MEditorToolbar;
class MEditorToolbarArrow;
class MLinearLayoutPolicy;
class MTopLevelOverlay;
class MWidget;

class MEditorToolbarPrivate
{
    Q_DECLARE_PUBLIC(MEditorToolbar)

public:
    explicit MEditorToolbarPrivate(MEditorToolbar *qq, const MWidget &followWidget);
    virtual ~MEditorToolbarPrivate();

    void init();
    void appear(bool autohide);
    void disappear();
    bool isAppeared() const;
    void handleActionAdded(QActionEvent *actionEvent);
    void handleActionRemoved(QActionEvent *actionEvent);
    void handleActionChanged(QActionEvent *actionEvent);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
    void updateArrow(MEditorToolbarArrow::ArrowDirection direction);
    void updateWidgetOrigin();
    void visibilityUpdated();
    void updateEditorItemVisibility();
    void showEditorItem();
    void hideEditorItem();

private:
    void _q_updateAvailableButtons();
    void _q_startAnimatedHide();

    MEditorToolbar * const q_ptr;

    QList<MButton *> buttons;

    QPointer<MTopLevelOverlay> overlay;
    const MWidget &followWidget;

    MLinearLayoutPolicy *buttonLayoutPolicy;
    MEditorToolbarArrow *arrow;

    bool buttonUpdateQueued;
    QTimer autohideTimer;
    QPropertyAnimation hideAnimation;

    EatMButtonGestureFilter *eatMButtonGestureFilter;
};

class EatMButtonGestureFilter : public QObject
{
    Q_OBJECT

public:
    EatMButtonGestureFilter(QObject *parent = 0);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
};


#endif // MEDITORTOOLBAR_P_H
