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

#ifndef UT_MEXTENSIONAREA_
#define UT_MEXTENSIONAREA_

#include <QObject>

class TestExtensionArea;
class MApplication;

class Ut_MExtensionArea : public QObject
{
    Q_OBJECT

private:
    // TestExtensionArea is derived from MExtensionArea
    TestExtensionArea *area;
    // MApplication instance required by MWidget.
    MApplication *app;

private slots:
    // Executed once before every test case
    void init();

    // Executed once after every test case
    void cleanup();

    // Executed once before first test case
    void initTestCase();

    // Executed once after last test case
    void cleanupTestCase();

    void testAddition();
    void testRemoval();
};

#endif // UT_MEXTENSIONAREA_
