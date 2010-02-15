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

#include <QGraphicsSceneMouseEvent>

#include "duiobjectmenuview_p.h"
#include "duibutton.h"
#include "duiviewcreator.h"
#include "duiobjectmenu.h"

#include "duipannableviewport.h"
#include "duilayout.h"
#include "duilinearlayoutpolicy.h"
#include "duigridlayoutpolicy.h"
#include <QGraphicsLinearLayout>

#include "duiwidgetaction.h"
#include "duiscenemanager.h"
#include "duiscalableimage.h"
#include "duiapplication.h"
#include "duiapplicationwindow.h"

void DuiObjectMenuViewPrivate::init()
{
    // create pannable viewport
    viewport = new DuiPannableViewport();

    // create container widget for pannable viewport
    container = new DuiWidget;
    viewport->setWidget(container);

    // create layout for controller, make it pannable
    QGraphicsLinearLayout *controllerLayout = new QGraphicsLinearLayout();
    controllerLayout->setContentsMargins(0, 0, 0, 0);
    controllerLayout->addItem(viewport);
    controller->setLayout(controllerLayout);

    // create layout policies for portrait & landscape orientation
    layout = new DuiLayout(container);
    portraitPolicy = new DuiLinearLayoutPolicy(layout, Qt::Vertical);
    portraitPolicy->setContentsMargins(0, 0, 0, 0);
    landscapePolicy = new DuiGridLayoutPolicy(layout);
    landscapePolicy->setContentsMargins(0, 0, 0, 0);

    layout->setLandscapePolicy(landscapePolicy);
    layout->setPortraitPolicy(portraitPolicy);
}


DuiObjectMenuView::DuiObjectMenuView(DuiObjectMenu *controller) :
    DuiSceneWindowView(* new DuiObjectMenuViewPrivate, controller)
{
    Q_D(DuiObjectMenuView);
    d->init();
}

DuiObjectMenuView::DuiObjectMenuView(DuiObjectMenuViewPrivate &dd, DuiObjectMenu *controller) :
    DuiSceneWindowView(dd, controller)
{
    Q_D(DuiObjectMenuView);
    d->init();
}

DuiObjectMenuView::~DuiObjectMenuView()
{
    DuiActionList actions = model()->actions();
    const int count = actions.count();
    for (int i = 0; i < count; ++i) {
        actionRemoved(actions.at(i));
    }
}

void DuiObjectMenuView::actionAdded(DuiAction *action)
{
    Q_D(DuiObjectMenuView);

    // show only if it is visible
    if (action->isVisible()) {

        // create button for this action
        DuiButton *button = new DuiButton(action->iconID(), action->text(), d->container);

        QObject::connect(button, SIGNAL(clicked(bool)), action, SIGNAL(triggered()));
        d->controller->connect(button, SIGNAL(clicked(bool)), SLOT(dismiss()));

        button->setEnabled(action->isEnabled());
        d->portraitPolicy->addItem(button);
        d->landscapePolicy->addItem(button, (d->buttons.count()) / 2, (d->buttons.count()) % 2);

        d->buttons.insert(action, button);

        QSize sceneSize;
        if (d->controller->sceneManager())
            sceneSize = d->controller->sceneManager()->visibleSceneSize();
        else if (DuiApplication::activeWindow())
            sceneSize = DuiApplication::activeWindow()->visibleSceneSize();
        else if (!DuiApplication::windows().isEmpty())
            sceneSize = DuiApplication::windows().at(0)->visibleSceneSize();

        d->portraitPolicy->setContentsMargins(0.0, 0.0, 0.0, 0.0);
        qreal offset = (sceneSize.height() - d->portraitPolicy->sizeHint(Qt::PreferredSize).height()) / 2.0;
        d->portraitPolicy->setContentsMargins(0.0, offset, 0.0, offset);

        d->landscapePolicy->setContentsMargins(0.0, 0.0, 0.0, 0.0);
        offset = (sceneSize.height() - d->landscapePolicy->sizeHint(Qt::PreferredSize).height()) / 2.0;
        d->landscapePolicy->setContentsMargins(0.0, offset, 0.0, offset);
    }
}

void DuiObjectMenuView::actionRemoved(DuiAction *action)
{
    Q_D(DuiObjectMenuView);

    DuiButton *button = d->buttons.value(action, NULL);
    if (button) {
        d->buttons.remove(action);
        delete button;
    }
}

void DuiObjectMenuView::actionModified(DuiAction *action)
{
    Q_D(DuiObjectMenuView);
    DuiButton *button = d->buttons.value(action, NULL);
    if (button) {
        if (!action->isVisible()) {
            actionRemoved(action);
        } else {
            // update button data accordingly
            button->setText(action->text());
            button->setIconID(action->iconID());
            button->setEnabled(action->isEnabled());
        }
    } else {
        // there is no button yet, action must've been invisible
        actionAdded(action);
    }
}

void DuiObjectMenuView::updateData(const QList<const char *> &modifications)
{
    Q_D(DuiObjectMenuView);

    foreach(const char * member, modifications) {
        if (member == DuiObjectMenuModel::Actions) {
            foreach(DuiAction * action, d->buttons.keys()) {
                actionRemoved(action);
            }

            DuiActionList actions = model()->actions();
            const int count = actions.count();
            for (int i = 0; i < count; ++i) {
                actionAdded(actions.at(i));
            }
        }
    }
}

void DuiObjectMenuView::setupModel()
{
    DuiSceneWindowView::setupModel();

    Q_D(DuiObjectMenuView);

    foreach(DuiAction * action, d->buttons.keys()) {
        actionRemoved(action);
    }

    connect(model(), SIGNAL(actionAdded(DuiAction *)),
            this, SLOT(actionAdded(DuiAction *)));
    connect(model(), SIGNAL(actionRemoved(DuiAction *)),
            this, SLOT(actionRemoved(DuiAction *)));
    connect(model(), SIGNAL(actionModified(DuiAction *)),
            this, SLOT(actionModified(DuiAction *)));

    DuiActionList actions = model()->actions();
    const int count = actions.count();
    for (int i = 0; i < count; ++i) {
        actionAdded(actions.at(i));
    }
}

void DuiObjectMenuView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //Do not pass the events through the menu window
    event->accept();
}

void DuiObjectMenuView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    Q_D(DuiObjectMenuView);
    d->controller->dismiss();
}

void DuiObjectMenuView::drawBackground(QPainter *painter, const QStyleOptionGraphicsItem *option) const
{
    Q_D(const DuiObjectMenuView);

    if (d->controller->sceneManager() == 0)
        return;

    Q_UNUSED(option);
    painter->setOpacity(style()->backgroundOpacity() * effectiveOpacity());

    QTransform oldTransform(painter->worldTransform());
    QTransform transform;
    transform.translate(pos().x(), pos().y());
    painter->setWorldTransform(transform);

    QSizeF size = boundingRect().size();
    if (d->controller->sceneManager()->orientation() != Dui::Landscape)
        size = QSizeF(size.height(), size.width());

    if (style()->backgroundImage()) {
        // TODO Use tiled bitmap drawing when it becomes available.
        style()->backgroundImage()->draw(0, 0, size.width(), size.height(), painter);
        painter->setWorldTransform(oldTransform);
    } else if (style()->backgroundColor().isValid()) {
        painter->fillRect(QRectF(QPointF(0, 0), size), QBrush(style()->backgroundColor()));
    }
}

DUI_REGISTER_VIEW_NEW(DuiObjectMenuView, DuiObjectMenu)
