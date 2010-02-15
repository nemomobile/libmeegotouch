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

#include "pt_widgetsgallery.h"

#include <QProcess>

#include <qtest.h>

void Pt_WidgetsGallery::startupTime()
{
    QProcess proc;
    const QString program = "widgetsgallery";
    const QStringList arguments = QStringList() << "-quitimmediately" <<  "-remote-theme";

    QBENCHMARK {
        proc.start(program, arguments);
        QVERIFY(proc.waitForStarted());
        QVERIFY(proc.waitForFinished());
        QCOMPARE(proc.exitStatus(), QProcess::NormalExit);
        QCOMPARE(proc.exitCode(), 0);
    }
}

QTEST_APPLESS_MAIN(Pt_WidgetsGallery)
