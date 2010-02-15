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

#include <QGraphicsWidget>
#include <QTest>
#include <QString>
#include <QPixmap>
#include <QSignalSpy>
#include <QGraphicsSceneMouseEvent>

#include <duiimagewidget.h>
#include "duicomponentdata.h"

#include "ut_duiimagewidget.h"

void Ut_DuiImageWidget::initTestCase()
{
    QImage img(qApp->applicationDirPath() + "/ut_duiimagewidget-test.png");

    if (! DuiComponentData::instance()) {
        char *argv = 0;
        int argc = 0;
        new DuiComponentData(argc, &argv, QString("ut_duiimagewidget"), NULL);
    }

    m_subject = new DuiImageWidget(&img);
    QVERIFY(m_subject != 0);
}

void Ut_DuiImageWidget::cleanupTestCase()
{
    delete m_subject;
    m_subject = 0;

    delete DuiComponentData::instance();
}

void Ut_DuiImageWidget::testConstructor_data()
{
    QTest::addColumn<QString>("fname");

    QTest::newRow("png") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png";
}

void Ut_DuiImageWidget::testConstructor()
{
    QFETCH(QString, fname);

    QSize s;

    QPixmap pixmap(fname);
    s = pixmap.size();

    // test load image files
    QImage img(fname);
    DuiImageWidget *myImage = new DuiImageWidget(&img);
    QCOMPARE(myImage->imageSize(), s);

    // test constructor from pixmap
    QPixmap *source = new QPixmap(s);
    source->fill(QColor(0, 0, 0, 0));
    DuiImageWidget *myImage3 = new DuiImageWidget(source);
    QCOMPARE(myImage3->imageSize(), s);
    delete myImage3;

    // test copy constructor
    DuiImageWidget *myImage4 = new DuiImageWidget(*myImage);
    QCOMPARE(myImage4->imageSize(), myImage->imageSize());
    delete myImage4;

    DuiImageWidget myImage5 = *myImage;
    QCOMPARE(myImage5.imageSize(), myImage->imageSize());

    delete source;
    delete myImage;
}

void Ut_DuiImageWidget::testImageDataSize_data()
{
    QTest::addColumn<QString>("fname");

    QTest::newRow("png") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png";
}

void Ut_DuiImageWidget::testImageDataSize()
{
    QFETCH(QString, fname);

    QImage img(fname);
    DuiImageWidget *myImage = new DuiImageWidget(&img);

    QSize imgSize;
    QPixmap pixmap(fname);
    imgSize = pixmap.size();

    const QPixmap *p = myImage->pixmap();
    QCOMPARE(imgSize, p->size());
    QCOMPARE(imgSize, myImage->imageSize());

    // test setPixmap
    myImage->setPixmap(pixmap);
    p = myImage->pixmap();
    QCOMPARE(pixmap.toImage(), p->toImage());

    // test setImage
    QImage image = pixmap.toImage();
    myImage->setImage(image);
    p = myImage->pixmap();
    QCOMPARE(image, p->toImage());

//     // test imageDataSize after set crop section
//     QSizeF half = imgSize*0.5;
//     myImage->setCropSize(half);
//     QCOMPARE(half, myImage->imageDataSize());

    delete myImage;
}

void Ut_DuiImageWidget::testSetZoomFactor_data()
{
    QTest::addColumn<QString>("fname");
    QTest::addColumn<qreal>("factor");
    QTest::addColumn<qreal>("f2");

    QTest::newRow("png1") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << static_cast<qreal>(1.0) << static_cast<qreal>(2.0);
    QTest::newRow("png2") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << static_cast<qreal>(0.0001) << static_cast<qreal>(2.0);
}

void Ut_DuiImageWidget::testSetZoomFactor()
{
    QFETCH(QString, fname);
    QFETCH(qreal, factor);
    QFETCH(qreal, f2);

    Q_UNUSED(factor)
    Q_UNUSED(f2)

    QImage img(fname);
    DuiImageWidget *myImage = new DuiImageWidget(&img);
    myImage->setZoomFactor(factor);

    qreal fx, fy;
    myImage->zoomFactor(&fx, &fy);

    QSize size = myImage->imageSize();
    if (size.width() * factor > 2.0) {
        QCOMPARE(fx, factor);
        QCOMPARE(fy, factor);

        // test zoom in & zoom out
        myImage->zoomIn();
        myImage->zoomFactor(&fx, &fy);

        QCOMPARE(fx, factor * 2.0f);
        QCOMPARE(fy, factor * 2.0f);

        myImage->zoomOut();
        myImage->zoomFactor(&fx, &fy);

        QCOMPARE(fx, factor);
        QCOMPARE(fy, factor);

        // test AspectRatioMode
        myImage->setZoomFactor(factor, f2);
        QCOMPARE(Qt::IgnoreAspectRatio, myImage->aspectRatioMode());

    } else {
        myImage->setZoomFactor(factor, factor);
        myImage->zoomFactor(&fx, &fy);

        QVERIFY(fx != factor);
        QVERIFY(fy != factor);
    }

    delete myImage;
}

void Ut_DuiImageWidget::testSetNegativeZoomFactor()
{
    qreal posvalue = 2.0f;
    qreal negvalue = -2.0f;
    qreal zx, zy;

    QImage img(qApp->applicationDirPath() + "/ut_duiimagewidget-test.png");
    DuiImageWidget *myImage = new DuiImageWidget(&img);
    myImage->setZoomFactor(posvalue, posvalue);
    myImage->zoomFactor(&zx, &zy);

    QVERIFY(qAbs(zx - posvalue) < 1e-3);
    QVERIFY(qAbs(zy - posvalue) < 1e-3);

    myImage->setZoomFactor(negvalue, negvalue);
    myImage->zoomFactor(&zx, &zy);

    QVERIFY(qAbs(zx - posvalue) < 1e-3);
    QVERIFY(qAbs(zy - posvalue) < 1e-3);

    delete myImage;
}

void Ut_DuiImageWidget::testSetCropSize_data()
{
    QTest::addColumn<QString>("fname");
    QTest::addColumn<QSizeF>("crop");

    QTest::newRow("png1") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << QSizeF(-10.0, -10.0);
    QTest::newRow("png2") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << QSizeF(10.0, 10.0);
    QTest::newRow("png3") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << QSizeF(10000.0, 10000.0);
}

void Ut_DuiImageWidget::testSetCropSize()
{
    QFETCH(QString, fname);
    QFETCH(QSizeF, crop);

    QImage img(fname);
    DuiImageWidget *myImage = new DuiImageWidget(&img);

    QRectF rect(QPointF(0.0, 0.0), crop);
    myImage->setCrop(rect);

    QSize imgSize = myImage->imageSize();
    QSizeF t = myImage->crop().size();

    if (crop.width() < 0.0)
        QCOMPARE(t, QSizeF(0.0, 0.0));
    else if (crop.width() > imgSize.width())
        QCOMPARE(t, QSizeF(imgSize));
    else
        QCOMPARE(t, crop);

    delete myImage;
}

void Ut_DuiImageWidget::testSetCropTopLeftPoint_data()
{
    QTest::addColumn<QString>("fname");
    QTest::addColumn<QPointF>("topLeft");

    QTest::newRow("png1") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << QPointF(-10.0, -10.0);
    QTest::newRow("png2") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << QPointF(-1.0, -1.0);
    QTest::newRow("png3") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << QPointF(0.0, 0.0);
    QTest::newRow("png4") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << QPointF(10.0, 10.0);
    QTest::newRow("png5") << qApp->applicationDirPath() + "/ut_duiimagewidget-test.png" << QPointF(10000.0, 10000.0);
}

void Ut_DuiImageWidget::testSetCropTopLeftPoint()
{
    QFETCH(QString, fname);
    QFETCH(QPointF, topLeft);

    QImage img(fname);
    DuiImageWidget *myImage = new DuiImageWidget(&img);

    QRectF rect(topLeft, QSize(10.0, 10.0));
    myImage->setCrop(rect);

    QPointF point = myImage->crop().topLeft();
    QSize imgSize = myImage->imageSize();

    // This is default value in DuiImageWidget
    if (topLeft.x() < 0.0 || topLeft.x() < 0.0)
        QCOMPARE(point, QPointF(-1.0, -1.0));
    else if (topLeft.x() > imgSize.width() || topLeft.y() > imgSize.height())
        QCOMPARE(point, QPointF(-1.0, -1.0));
    else
        QCOMPARE(point, topLeft);

    delete myImage;
}

void Ut_DuiImageWidget::testImageNotExist_data()
{
    QTest::addColumn<QString>("fname");
    QTest::newRow("png") << "doesnotexist.png";
}

void Ut_DuiImageWidget::testImageNotExist()
{
    QFETCH(QString, fname);
    QImage img(fname);
    DuiImageWidget *myImage = new DuiImageWidget(&img);

    QSize imageSize = myImage->imageSize();
    QVERIFY(imageSize.isEmpty());

    delete myImage;
}


QTEST_MAIN(Ut_DuiImageWidget);

