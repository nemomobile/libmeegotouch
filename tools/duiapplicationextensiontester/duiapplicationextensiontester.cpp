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

/*
 * Application extension testing tool.
 */

#include "duiapplicationextensiontester.h"

#include <QDebug>
#include <getopt.h>
#include <QString>
#include <QGraphicsLinearLayout>
#include <DuiApplication>
#include <DuiApplicationWindow>
#include <DuiApplicationPage>
#include <DuiApplicationExtensionArea>
#include <DuiMessageBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>


ApplicationExtensionPage::ApplicationExtensionPage(const char *metaDataFileName) :
    metaDataFileName(metaDataFileName)
{
    QString appName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    QString path = QDir::homePath() + "/.config/" + appName;
    if (!QDir::root().exists(path)) {
        QDir::root().mkpath(path);
    }

    QFile f(path + "/extensionarea.data");
    f.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream ts(&f);
    ts << "[1]\ndesktopFile=" << metaDataFileName << "\nprivate\\layoutIndex=0\n";
}

void ApplicationExtensionPage::createContent()
{
    DuiWidget *panel = centralWidget();
    QGraphicsLinearLayout *vbox = new QGraphicsLinearLayout(Qt::Vertical);
    panel->setLayout(vbox);

    DuiApplicationExtensionArea *extArea = new DuiApplicationExtensionArea("extensionarea");
    vbox->addItem(extArea);
}

void usage(const char *progName)
{
    qWarning("Usage: %s METADATAFILENAME", progName);
    qWarning("  metadatafilename                   Defines the metadata file of the extension");
}

int main(int argc, char **argv)
{
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            { "help", 0, NULL, 'h' },  /* --help or -h  */
            { 0,    0,   0,   0 }
        };
        int c = getopt_long_only(argc, argv, ":h", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
        case 'h':
            usage(argv[0]);
            return -1;
        default:
            break;
        }
    }

    if (optind >= argc || QString(argv[optind]).isEmpty()) {
        usage(argv[0]);
        return -1;
    }

    DuiApplication m_app(argc, argv);
    DuiApplicationWindow window;
    ApplicationExtensionPage page(argv[optind]);
    window.show();
    page.appear();

    return m_app.exec();
}
