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

#ifndef LISTPAGE_H
#define LISTPAGE_H

#include <DuiContainer>
#include <QHash>

#include "timedemopage.h"

class TemplatePage;
class QGraphicsLayoutItem;
class DuiGridLayoutPolicy;
class DuiLinearLayoutPolicy;
class DuiLabel;
class DuiButton;
class QSettings;

class MyContainer : public DuiContainer
{
public:
    MyContainer(QGraphicsItem *parent = 0);
    virtual ~MyContainer();

    DuiGridLayoutPolicy  *landscapePolicy;
    DuiLinearLayoutPolicy  *portraitPolicy;

    void addItem(QGraphicsLayoutItem *button);

};

class ListPage : public TimedemoPage
{
    Q_OBJECT

public:
    ListPage(const QString &title = "");
    virtual ~ListPage();
    virtual QString timedemoTitle();

    virtual void createContent();

    void addPage(TemplatePage *page);
    int pageCount() const;
    TemplatePage *findPageByIndex(int index) const;
    TemplatePage *findPageByTimedemoTitle(const QString& title) const;
    void showPageByTimedemoTitle(const QString& name);

    static DuiGridLayoutPolicy *createAndSetupGridPolicy(DuiWidget *panel);

    void setInitialPageToShow(const QString& initialPageToShow);

    QList<TemplatePage *> pages;

public slots:
    void handleListItemClick();
    void showThemeSelectionDialog();
    void showPageByIndex(int index);
    void showOrientationSelectionDialog();
    void toggleFps();
    void showInitialPage();

protected:
    virtual void retranslateUi();

private:
    void populateLayout();
    void showPage(DuiApplicationPage *page);

private:
    QList<DuiLabel *> groupLabels;
    QList<DuiContainer *> containers;

    DuiApplicationPage *shownPage;
    DuiLinearLayoutPolicy *policy;

    QHash<DuiButton *, TemplatePage *> buttons;

    QString initialPageToShow;
};

#endif
