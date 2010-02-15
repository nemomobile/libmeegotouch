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

#include <DuiButton>
#include <DuiLabel>
#include <DuiTheme>
#include <DuiLocale>
#include <DuiLinearLayoutPolicy>
#include <DuiGridLayoutPolicy>
#include <DuiProgressIndicator>
#include <DuiApplication>
#include <DuiDebug>
#include <DuiLayout>
#include <QString>
#include <QTime>
#include <duiapplicationwindow.h>
#include "anotherpage.h"

AnotherPage::AnotherPage()
{
    setTitle("Visibility Page");
    setObjectName("visibilitypage");

    labelBgColors << "<BODY BGCOLOR=\"#00FF00\">" << "<BODY BGCOLOR=\"#FF0000\">"
                  << "<BODY BGCOLOR=\"#0000FF\">" << "<BODY BGCOLOR=\"orange\">";

    labelBgColorIndex = 0;
    hiddenCount = 0;

    logLabelRows << "" << "" << "" << "";
}

AnotherPage::~AnotherPage()
{
}

void AnotherPage::createContent()
{
    DuiLayout *layout = new DuiLayout();
    centralWidget()->setLayout(layout);

    l_policy = new DuiGridLayoutPolicy(layout);
    l_policy->setContentsMargins(32, 32, 32, 32);
    l_policy->setRowSpacing(1, 32.0f);
    l_policy->setSpacing(36);

    DuiLabel *label1 = new DuiLabel("This Label's Visibility Log:", this);
    label1->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
    label1->setObjectName("Label");

    windowHiddenLabel = new DuiLabel(("Window hidden: 00:00:00"), this);
    windowVisibleLabel = new DuiLabel(("Window visible: 00:00:00"), this);

    panLabel = new DuiLabel((labelBgColors.at(labelBgColorIndex++) + "<BR>Pan me out<BR></BODY>"), this);
    panLabel->setObjectName("panLabel");

    logLabel = new DuiLabel("", this);
    fillLogLabel("<FONT COLOR=\"#00FFFF\">" + QTime::currentTime().toString("hh:mm:ss") + " Visible</FONT>");

    DuiButton *button = new DuiButton("Clear Logs");
    connect(button, SIGNAL(clicked()), this, SLOT(clearLog()));

    connect(panLabel, SIGNAL(displayEntered()), this, SLOT(itemVisible()));
    connect(panLabel, SIGNAL(displayExited()),  this, SLOT(itemHidden()));
    connect(label1,   SIGNAL(displayEntered()), this, SLOT(itemVisible()));
    connect(label1,   SIGNAL(displayExited()),  this, SLOT(itemHidden()));
    DuiApplicationWindow *window = DuiApplication::activeApplicationWindow();
    connect(window,   SIGNAL(displayExited()),  this, SLOT(windowHidden()));
    connect(window,   SIGNAL(displayEntered()), this, SLOT(windowVisible()));

    for (int i = 0; i < LABEL_COUNT; i++) {
        labelList[i] = new DuiLabel(("<BODY BGCOLOR=\"#0000FF\"><CENTER>" + QString(i + 'A') + "</CENTER></BODY>"), this);
        labelList[i]->setObjectName("labelList" + QString(i + 'A'));
        connect(labelList[i], SIGNAL(displayEntered()), this, SLOT(itemVisible()));
        connect(labelList[i], SIGNAL(displayExited()), this, SLOT(itemHidden()));
        l_policy->addItem(labelList[i], 6 + i, 0, 1, 3);
    }

    l_policy->addItem(label1,  0, 0, 1, 2);
    l_policy->addItem(logLabel, 1, 0, 3, 2);
    l_policy->addItem(button, 4, 2, 1, 1);
    l_policy->addItem(panLabel, 3, 2, 1, 1);
    l_policy->addItem(windowHiddenLabel, 0, 2);
    l_policy->addItem(windowVisibleLabel, 1, 2);
    // Return to initial state in case of LazyShutdown
    connect(DuiApplication::instance(), SIGNAL(prestartRestored()), this, SLOT(clearLog()));
}

void AnotherPage::clearLog()
{
    for (int i = 0; i < 4; i++) {
        fillLogLabel("");
    }
    labelBgColorIndex = 0;
    hiddenCount = 0;
    panLabel->setText(labelBgColors.at(labelBgColorIndex++ % 4) + "<BR>Pan me out<BR></BODY>");
    windowHiddenLabel->setText("Window hidden: 00:00:00");
    windowVisibleLabel->setText("Window visible: 00:00:00");
}

void AnotherPage::fillLogLabel(QString text)
{
    logLabelRows.replace(3, logLabelRows.at(2));
    logLabelRows.replace(2, logLabelRows.at(1));
    logLabelRows.replace(1, logLabelRows.at(0));
    logLabelRows.replace(0, text);

    logLabel->setText("<br><br><br>" + logLabelRows.join("<br>"));
}

void AnotherPage::itemHidden()
{
    duiDebug("") << sender()->objectName() << "HIDDEN";
    if (sender()->objectName() == "Label") {
        fillLogLabel("<FONT COLOR=\"#FFFFFF\">" + QTime::currentTime().toString("hh:mm:ss") + " Hidden</FONT>");
    } else if (sender()->objectName() == "panLabel") {
        hiddenCount++;
        panLabel->setText(labelBgColors.at(labelBgColorIndex++ % 4) + "<BR>Pan me out<BR>(Hidden " + QString().setNum(hiddenCount) + "X)<BR></BODY>");
    } else if (sender()->objectName().leftRef(9) == "labelList") {
        QString header = "Visible: ";

        for (int i = 0; i < LABEL_COUNT; i++) {
            if (labelList[i]->isOnDisplay()) {
                header = header + QString(i + 'A');
            }
        }
        setTitle(header);
    }
}

void AnotherPage::itemVisible()
{
    duiDebug("") << sender()->objectName() << "Visible";
    if (sender()->objectName() == "Label") {
        fillLogLabel("<FONT COLOR=\"#00FFFF\">" + QTime::currentTime().toString("hh:mm:ss") + " Visible<</FONT>");
    } else if (sender()->objectName().leftRef(9) == "labelList") {
        QString header = "Visible: ";

        for (int i = 0; i < LABEL_COUNT; i++) {
            if (labelList[i]->isOnDisplay()) {
                header = header + QString(i + 'A');
            }
        }

        setTitle(header);
    }
}

void AnotherPage::windowHidden()
{
    duiDebug("") << "lifecycle: Window hidden";
    windowHiddenLabel->setText("Window hidden: " +  QTime::currentTime().toString("hh:mm:ss"));
}

void AnotherPage::windowVisible()
{
    duiDebug("") << "lifecycle: Window visible";
    windowVisibleLabel->setText("Window visible: " +  QTime::currentTime().toString("hh:mm:ss"));
}

void AnotherPage::exitDisplayEvent()
{
    duiDebug("") << "lifecycle: AnotherPage hidden";
}

void AnotherPage::enterDisplayEvent()
{
    duiDebug("") << "lifecycle: AnotherPage visible";
}


