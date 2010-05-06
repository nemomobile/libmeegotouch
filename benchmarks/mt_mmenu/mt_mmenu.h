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

#ifndef MT_MMENU_H
#define MT_MMENU_H

#include <QtTest/QtTest>
#include <QObject>

class Mt_MMenu : public QObject
{
    Q_OBJECT

private:
    void memoryBenchmark(const QString &view);

private slots:
    void initTestCase();
    void cleanupTestCase();

    void memoryMMenuView();
};

#endif

