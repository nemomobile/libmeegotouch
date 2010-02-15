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

#ifndef UT_DUIREMOTEACTION_H
#define UT_DUIREMOTEACTION_H

#include <QtTest/QtTest>
#include <QObject>
#include <QString>
#include <QList>
#include <QVariant>

// the real unit/DuiRemoteAction class declaration
#include <duiremoteaction.h>

Q_DECLARE_METATYPE(DuiRemoteAction *);

class Ut_DuiRemoteAction : public QObject
{
    Q_OBJECT

public:
    static bool captureCalls;
    static QString callServiceName;
    static QString callObjectPath;
    static QString callInterface;
    static QList<QString> callMethods;
    static QList< QList<QVariant> > callArguments;

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void testSerialization();
    void testTrigger();
};

#endif
