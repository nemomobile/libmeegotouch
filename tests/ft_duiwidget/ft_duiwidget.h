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

#ifndef FT_DUIWIDGET_H
#define FT_DUIWIDGET_H

#include <QObject>
#include <QtTest/QtTest>
#include <QString>
#include "duiapplication.h"
#include <QList>
#include "duiwidget.h"
#include <QGraphicsWidget>

class Ft_DuiWidget : public QObject
{
    Q_OBJECT;

public:
    Ft_DuiWidget();

private slots:
    void initTestCase();
    void resizeAndSizeLookup();
    void paintWidgetWithoutForcedResize();
    void paintWidgetWithForcedResize();
    void constructAndDeleteWidgets();
    void initialSize();

private:
    DuiApplication *app;
    void createWidgetsUnderTest();
    QList<QGraphicsWidget *> widgetsUnderTest;
    QSizeF paintedSize(QGraphicsWidget *widget);

};

#endif

