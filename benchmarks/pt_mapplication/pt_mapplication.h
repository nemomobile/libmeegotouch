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

#ifndef PT_QAPPLICATION_H
#define PT_QAPPLICATION_H

#include <QObject>

/**
  * Test performance of the MApplication constructor.
  * The constructor is created in process and as part of a newly created process.
  *
  * The most timeconsuming parts of the constructor can be verified by the following tests:
  * - pt_qapplication
  * - pt_mcomponentdata
  */
class Pt_MApplication : public QObject
{
    Q_OBJECT
private slots:
    /**
      * Test how long it takes to launch an application which is quitting immediately.
      */
    void processCreation();

    /**
      * Test the performance of the mapplication constructor.
      *
      * This test creates a new process and thus includes process creation overhead.
      * Callgrind results are meaningless since the child process is not traced.
      */
    void processCreationAndConstructor();

    /**
      * Test the performance of the mapplication constructor.
      */
    void uncachedConstructor();

    /**
     * Execute the constructor a second time to evaluate caching possibilities.
     */
    void cachedConstructor();

private:
    /**
      * Executes the current programm with a given parameter.
      */
    void executeSelf(const QLatin1String &parameter);
};

#endif // PT_QAPPLICATION_H
