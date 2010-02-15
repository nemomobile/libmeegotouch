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

#ifndef PT_DUISCALABLEIMAGE_H
#define PT_DUISCALABLEIMAGE_H

#include <QtTest/QtTest>
#include <QObject>

class Pt_DuiScalableImage : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void initTestCase();
    void cleanupTestCase();

    void paintScalablePerformance();
    void paintScalablePerformance_data();

    void paintPixmapPerformance();
    void paintPixmapPerformance_data();

};

#endif
