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

#ifndef FT_MPROGRESSINDICATOR_H
#define FT_MPROGRESSINDICATOR_H

#include <QObject>
#include <QtTest/QtTest>
#include <QString>

class MProgressIndicator;

class Ft_MProgressIndicator : public QObject
{
    Q_OBJECT

private:
    MProgressIndicator *m_subject;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void setValue();
    void setMinimum();
    void setMaximum();
    void unknown();
    void reset();
};

#endif

