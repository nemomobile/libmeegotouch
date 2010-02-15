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

#include <QtTest/QtTest>
#include <QList>
#include <QGraphicsItem>
#include <QPainter>
#include <QTime>
#include <duiapplication.h>
#include <duideviceprofile.h>
#include <duitheme.h>
#include "ut_duiscene.h"
#include <DuiButton>


#define MAX_PARAMS 10

void Ut_DuiScene::initTestCase()
{


}
void Ut_DuiScene::cleanupTestCase()
{
}


void Ut_DuiScene::init()
{
    QChar sep(' ');
    char *argv[MAX_PARAMS];
    int x = 0;
    QString params("./ut_duiscene -software -show-br -show-fps -show-size -show-position");

    QStringList list = params.split(sep);
    QStringListIterator it(list);
    while (it.hasNext() && x < MAX_PARAMS)  {
        argv[x++] = strdup(it.next().toLocal8Bit().constData());
    }
    x = 6;
    app = new DuiApplication(x, argv);

    m_subject = new DuiScene();
}

void Ut_DuiScene::cleanup()
{
    delete m_subject;
    m_subject = 0;

    delete app;
    app = 0;
}

void Ut_DuiScene::drawForeground()
{
    QPixmap *p = new QPixmap(300, 300);
    p->fill(QColor(255, 255, 255, 0));
    QPainter *myPainter = new QPainter(p);

    m_subject->drawForeground(myPainter, QRect(0, 0, 300, 300));
    QImage img1 = p->toImage();

    m_subject->addItem(new DuiButton("foobar"));
    m_subject->drawForeground(myPainter, QRect(0, 0, 300, 300));
    QImage img2 = p->toImage();
    QVERIFY(img1 != img2);

    QVERIFY(img1 == img1);
}

QTEST_APPLESS_MAIN(Ut_DuiScene)
