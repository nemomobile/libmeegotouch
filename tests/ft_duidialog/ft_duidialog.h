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

#ifndef FT_DUIDIALOG_H
#define FT_DUIDIALOG_H

#include <QObject>
#include <QtTest/QtTest>
#include <QString>

class DuiDialog;

class Ft_DuiDialog : public QObject
{
    Q_OBJECT

public slots:
    void timer();

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void test_signals();
    void test_eventloop();
    void test_other();
    void testDismissDialogInEventLoop();
private:

    DuiDialog      *m_subject;
};

#endif

