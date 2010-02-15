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

#include "ut_duinavigationbar.h"

#include <DuiApplication>
#include <DuiNavigationBar>
#include <DuiToolBar>

#include <QGraphicsSceneMouseEvent>

DuiApplication *app;

void Ut_DuiNavigationBar::initTestCase()
{
    static int argc = 1;
    static char *app_name[1] = { (char *) "./ut_duinavigationbar" };
    app = new DuiApplication(argc, app_name);
}

void Ut_DuiNavigationBar::cleanupTestCase()
{
    delete app;
}

void Ut_DuiNavigationBar::init()
{
    subject = new DuiNavigationBar();
}

void Ut_DuiNavigationBar::cleanup()
{
    delete subject;
    subject = 0;
}

void Ut_DuiNavigationBar::testConstructor()
{
    QString name("test");
    subject->setViewMenuIconID(name);
    QCOMPARE(subject->viewMenuIconID(), name);

}

void Ut_DuiNavigationBar::testToolbar()
{
    QVERIFY(!subject->model()->toolBar());

    DuiToolBar *toolbar = new DuiToolBar();
    subject->dockToolBar(toolbar);
    QCOMPARE(subject->model()->toolBar(), toolbar);

    delete toolbar;
    subject->undockToolBar();
    QVERIFY(!subject->model()->toolBar());
}

void Ut_DuiNavigationBar::testViewMenuIconID()
{
    QString iconID("Icon-back");

    // this is the default value from model
    QCOMPARE(subject->viewMenuIconID(), QString(""));

    subject->setViewMenuIconID(iconID);
    QCOMPARE(subject->viewMenuIconID(), iconID);
}

/*
 * In this test I want to verify that when someone taps onto the navigation bar,
 * widgets on the application page behind it won't get activated.
 *
 * In practice this means that the navigation bar should handle/accept ALL the
 * input events that come to him.
 */
void Ut_DuiNavigationBar::noMouseEventPropagation()
{
    bool handledEvent;
    QGraphicsSceneMouseEvent *mouseEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);

    mouseEvent->setButtons(Qt::LeftButton);

    // Add it to the scene
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addItem(subject);

    handledEvent = subject->scene()->sendEvent(subject, mouseEvent);
    scene->removeItem(subject);
    delete scene;

    QVERIFY(mouseEvent->isAccepted());
    QVERIFY(handledEvent);
}

QTEST_APPLESS_MAIN(Ut_DuiNavigationBar)
