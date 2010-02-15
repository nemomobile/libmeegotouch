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

#include "ut_duisubdatastore.h"
#include "duisubdatastore.h"
#include "../stubs/mockdatastore.h"

static const QString PREFIX("testPrefix");


void Ut_DuiSubDataStore::initTestCase()
{
}

void Ut_DuiSubDataStore::cleanupTestCase()
{
}

void Ut_DuiSubDataStore::init()
{
    testBaseStore = new MockDataStore;
    testableDataStore = new DuiSubDataStore(PREFIX, *testBaseStore);
}

void Ut_DuiSubDataStore::cleanup()
{
    delete testableDataStore;
    delete testBaseStore;
}

void Ut_DuiSubDataStore::testPrefixSet()
{
    QCOMPARE(testableDataStore->prefix(), PREFIX);
}

void Ut_DuiSubDataStore::testCreateValue()
{
    QCOMPARE(testBaseStore->contains(PREFIX + QString("foo")), false);
    testableDataStore->createValue(QString("foo"), QString("bar1"));
    QCOMPARE(testBaseStore->value(PREFIX + QString("foo")).toString(), QString("bar1"));
}

void Ut_DuiSubDataStore::testSetValue()
{
    QVERIFY(!testableDataStore->setValue(QString("foo"), QString("bar")));
    QCOMPARE(testBaseStore->contains(PREFIX + QString("foo")), false);

    testableDataStore->createValue(QString("foo"), QString("bar1"));
    QVERIFY(testableDataStore->setValue(QString("foo"), QString("bar2")));
    QCOMPARE(testBaseStore->value(PREFIX + QString("foo")).toString(), QString("bar2"));
}

void Ut_DuiSubDataStore::testValue()
{
    QVERIFY(testableDataStore->createValue(QString("foo"), QString("bar")));
    QCOMPARE(testableDataStore->value(QString("foo")).toString(), QString("bar"));
}

void Ut_DuiSubDataStore::testAllKeys()
{
    testableDataStore->createValue(QString("foo"), QString("bar"));
    testableDataStore->createValue(QString("foo2"), QString("bar"));
    QStringList keys = testableDataStore->allKeys();
    QCOMPARE(keys.size(), 2);
    QVERIFY(keys.contains(QString("foo")));
    QVERIFY(keys.contains(QString("foo2")));
}

void Ut_DuiSubDataStore::testRemove()
{
    testableDataStore->createValue(QString("foo"), QString("bar"));
    testableDataStore->remove(QString("foo"));
    QCOMPARE(testBaseStore->contains(PREFIX + QString("foo")), false);
}

void Ut_DuiSubDataStore::testClear()
{
    testableDataStore->createValue(QString("foo"), QString("bar"));
    testableDataStore->createValue(QString("foo2"), QString("bar"));
    testBaseStore->createValue(QString("foo4"), QString("bar"));
    testableDataStore->clear();
    QCOMPARE(testBaseStore->contains(PREFIX + QString("foo")), false);
    QCOMPARE(testBaseStore->contains(PREFIX + QString("foo2")), false);
    QCOMPARE(testBaseStore->contains(QString("foo4")), true);
}

void Ut_DuiSubDataStore::testContains()
{
    testableDataStore->createValue(QString("foo"), QString("bar"));
    testBaseStore->createValue(QString("foo2"), QString("bar"));
    QCOMPARE(testableDataStore->contains(QString("foo")), true);
    QCOMPARE(testableDataStore->contains(QString("foo2")), false);
}

void Ut_DuiSubDataStore::testValueChangedSignal()
{
    QSignalSpy spy(testableDataStore, SIGNAL(valueChanged(QString, QVariant)));

    testBaseStore->createValue(QString("foo"), QString("bar"));
    QCOMPARE(spy.count(), 0);

    testBaseStore->createValue(PREFIX + QString("foo"), QString("bar"));
    QCOMPARE(spy.count(), 1);
}

QTEST_APPLESS_MAIN(Ut_DuiSubDataStore)
