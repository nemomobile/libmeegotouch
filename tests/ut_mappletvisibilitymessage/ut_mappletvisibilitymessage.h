/***************************************************************************
**
** Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef UT_MAPPLETVISIBILITYMESSAGE_H
#define UT_MAPPLETVISIBILITYMESSAGE_H

#include <QObject>

class QBuffer;
class QDataStream;
class MAppletVisibilityMessage;

class Ut_MAppletVisibilityMessage : public QObject
{
    Q_OBJECT

private:
    QBuffer *buffer;
    QDataStream *stream;
    MAppletVisibilityMessage *message;

private slots:
    void init();
    void cleanup();

    void testType();
    void testVisibilityProperty();
    void testSerialization();
    void testUnserialization();
};

#endif // UT_MAPPLETVISIBILITYMESSAGE_H
