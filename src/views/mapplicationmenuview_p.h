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

#ifndef MAPPLICATIONMENUVIEW_P_H
#define MAPPLICATIONMENUVIEW_P_H

#include <QObject>
#include <QAbstractListModel>
#include <QAction>
#include <QPointer>
#include <QList>
#include <QGraphicsLinearLayout>

#include "mlistitem.h"
#include "mabstractcellcreator.h"

class MApplicationMenu;
class MApplicationMenuView;
class MList;
class MPannableViewport;
class MStylableWidget;
class MLabel;
class MImageWidget;

class MActionsItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        ActionRole = Qt::UserRole + 1
    };

    MActionsItemModel(QGraphicsWidget* actionsProvider, QObject* parent = NULL);
    virtual ~MActionsItemModel();

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex & = QModelIndex()) const {
        return filteredActions.count();
    }

public slots:
    void freeze();
    void unfreeze();

private:
    void update();
    bool eventFilter(QObject *, QEvent *);

    QGraphicsWidget* actionsProvider;
    QList< QPointer<QAction> > filteredActions;
    bool needsUpdate;
    bool frozen;
};

class MMenuListItem : public MListItem
{
public:
    enum ItemStyle {
        SingleTitle,
        IconTitle,
        CustomWidget
    };

    MMenuListItem(QGraphicsItem* parent = 0);
    virtual ~MMenuListItem();

    void setTitle(const QString& title);
    void setIconID(const QString& id);
    void setCustomWidget(MWidget* widget);
    MWidget* customWidget() { return widget; }
    void updateLayout(ItemStyle itemStyle);
    void setTitleStyleName(const QString &styleName);

private:
    MImageWidget* icon;
    MLabel* title;
    QPointer<MWidget> widget;
    QString titleStyleName;
};

class MApplicationMenuCellCreator : public MCellCreator
{
public:
    MApplicationMenuCellCreator(MList* list);
    MWidget* createCell(const QModelIndex& index, MWidgetRecycler& recycler) const;
    void updateCell(const QModelIndex& index, MWidget * cell) const;
    QSizeF cellSize() const { return size; }
    void setItemStyleName(const QString &styleName);
    void setItemTitleStyleName(const QString &styleName);

private:
    static bool isComboBox(MWidget* widget);
    MWidget* extractWidget(QAction* action) const;
    QSizeF size;
    QString itemStyleName;
    QString itemTitleStyleName;
};

class MApplicationMenuLayout : public QObject, public QGraphicsLinearLayout
{
    Q_OBJECT
public:
    MApplicationMenuLayout(QGraphicsLayoutItem *parent = 0);

Q_SIGNALS:
    void geometrySet();

protected:
    //! \reimp
    virtual void setGeometry(const QRectF &rect);
    //! \reimp_end

};

class MApplicationMenuViewPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(MApplicationMenuView)

public:
    MApplicationMenuViewPrivate(MApplicationMenu *menu);
    virtual ~MApplicationMenuViewPrivate();
    void init();
    void updateStyleNames();
    void setupListModel();

private slots:
    void actionTriggered(const QModelIndex &index);
    void resetListPosition();
    void checkIfNewControllerGeometryNeeded();
    void onAppeared();
    void onControllerGeometryChanged();

private:
    MApplicationMenuView *q_ptr;
    MApplicationMenu *controller;
    MPannableViewport* actionCommandViewport;
    MStylableWidget *topArea;
    MList* list;
    MApplicationMenuCellCreator *cellCreator;

    QSizeF preferredSizeWhenLastChecked;
    QSizeF preferredSizeWhenControllerGeometryLastSet;

#ifdef UNIT_TEST
    friend class Ut_MApplicationMenu;
    friend class Ut_MApplicationMenuView;
#endif
};

#endif
