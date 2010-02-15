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

#include "duibasiclayoutanimation.h"
#include "duibasiclayoutanimation_p.h"
#include "duibasiclayoutanimationstyle.h"
#include "duilayouthelper_p.h"

#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsScene>
#include <QEasingCurve>
DuiBasicLayoutAnimation::DuiBasicLayoutAnimation(DuiLayout *layout) :
    DuiLayoutAnimation(*(new DuiBasicLayoutAnimationPrivate()), layout)
{
    Q_D(DuiBasicLayoutAnimation);
    d->q_ptr = this;
    setLoopCount(-1);
}

DuiBasicLayoutAnimation::~DuiBasicLayoutAnimation() { }

void DuiBasicLayoutAnimation::animate(DuiItemState &item)
{
    Q_D(DuiBasicLayoutAnimation);

    QRectF target(item.targetGeometry());
    QRectF start(item.sourceGeometry());
    qreal delta_progress = d->recordedTimeSinceLastUpdate / style()->duration(); //Both times are in milliseconds

    //First animate the geometry if needed
    if (item.geometryProgress() != 1) {
        //animate the geometry
        qreal new_geometry_progress = qMin(item.geometryProgress() + delta_progress, (qreal)1.0); //between 0 and 1
        item.setGeometryProgress(new_geometry_progress);
        qreal geometry_animation_value = style()->geometryEasingCurve().valueForProgress(new_geometry_progress);
        if (item.geometryProgress() == 1) {
            item.item()->setGeometry(target);
        } else {
            QSizeF new_size = start.size() + (target.size() - start.size()) * geometry_animation_value;
            new_size = new_size.boundedTo(item.item()->maximumSize()).expandedTo(item.item()->minimumSize());
            QPointF new_position = start.center() - QPointF(new_size.width() / 2, new_size.height() / 2) + (target.center() - start.center()) * geometry_animation_value;
            QRectF new_geometry(new_position, new_size);
            if (item.item()->geometry() != new_geometry)
                item.item()->setGeometry(new_geometry);
        }
    }

    //Then animate the opacity if needed
    if (item.opacityProgress() != 1 && item.targetOpacity() != -1) { //-1 target opacity means to not animate anyway
        //animate the opacity
        qreal new_opacity_progress = qMin(item.opacityProgress() + delta_progress, (qreal)1.0); //between 0 and 1
        item.setOpacityProgress(new_opacity_progress);
        qreal opacity_animation_value = style()->opacityEasingCurve().valueForProgress(new_opacity_progress);

        qreal current_opacity = item.currentOpacity();
        if (current_opacity != -1) {
            qreal new_opacity = item.sourceOpacity() + (item.targetOpacity() - item.sourceOpacity()) * opacity_animation_value;
            new_opacity = qAbs(new_opacity); //make sure it is between 0 and 1
            if (new_opacity > 1)
                new_opacity = qMax((float)qAbs(2 - new_opacity), 1.0f);
            item.setCurrentOpacity(new_opacity);
        }
    }

    //Have we finished with this item now?
    if (item.opacityProgress() == 1 && item.geometryProgress() == 1) {
        if (item.isSet(DuiItemState::STATE_FLAG_TO_BE_HIDDEN)) {
            if (item.targetOpacity() != -1)
                item.setCurrentOpacity(1); //Restore the opacity to 1, since we are hiding it now anyway
            hideItemNow(item.item());
        }
        item.animationDone();
    }
}

void DuiBasicLayoutAnimation::layoutAnimationFinished()
{
    stop();
}

void DuiBasicLayoutAnimation::doItemHiddenAnimation(DuiItemState *itemstate)
{
    Q_ASSERT(itemstate->item());
    // for item hiding it should deflate from its current size to final-hiding-scale-factor
    qreal finalScaleFactor = style()->finalHidingScaleFactor();
    QRectF finalGeometry;
    if (finalScaleFactor == 1.0) {
        finalGeometry = itemstate->item()->geometry();
    } else {
        QSizeF size = itemstate->item()->effectiveSizeHint(Qt::MinimumSize);
        if (finalScaleFactor > 0) {
            size = size.expandedTo(itemstate->item()->geometry().size() * finalScaleFactor)
                   .boundedTo(itemstate->item()->effectiveSizeHint(Qt::MaximumSize));
        }
        finalGeometry = QRectF(QPointF(0, 0), size);
        finalGeometry.moveCenter(itemstate->item()->geometry().center());
    }

    if (itemstate->targetGeometry().size() != finalGeometry.size())
        itemstate->setTargetGeometry(finalGeometry);

    QGraphicsItem *graphicsItem = itemstate->item()->graphicsItem();
    if (graphicsItem) {
        qreal finalOpacity = style()->finalHidingOpacity();
        if (finalOpacity >= 0 && finalOpacity < 1) //If it ==1 there's no need to do anything
            itemstate->setTargetOpacity(finalOpacity);
    }
}
void DuiBasicLayoutAnimation::doItemShownAnimation(DuiItemState *itemstate)
{
    Q_ASSERT(itemstate->item());
    if (itemstate->isSet(DuiItemState::STATE_FLAG_SHOWING)) {
        //If the item is already visible, this can be called because we were currently doing a hide animation.
        //So just make sure that we make everything visible
        showItemNow(itemstate->item());
        QGraphicsItem *graphicsItem = itemstate->item()->graphicsItem();
        if (graphicsItem) {
            itemstate->setTargetOpacity(1);
            graphicsItem->setOpacity(1);
        }
        return;
    }
    //If the item that started this animation is to be added/shown, set the initial geometry
    qreal initialScaleFactor = style()->initialShowingScaleFactor();
    QRectF initialGeometry;

    if (initialScaleFactor == 1.0) {
        initialGeometry = itemstate->targetGeometry();
    } else {
        QSizeF size = itemstate->item()->effectiveSizeHint(Qt::MinimumSize);
        if (initialScaleFactor != 0.0) {
            size = size.expandedTo(itemstate->targetGeometry().size() * initialScaleFactor)
                   .boundedTo(itemstate->item()->effectiveSizeHint(Qt::MaximumSize));
        }
        initialGeometry = QRectF(QPointF(0, 0), size);
        initialGeometry.moveCenter(itemstate->targetGeometry().center());
    }

    itemstate->setGeometry(initialGeometry);

    showItemNow(itemstate->item());
    QGraphicsItem *graphicsItem = itemstate->item()->graphicsItem();
    if (graphicsItem) {
        qreal initialOpacity = style()->initialShowingOpacity();
        if (initialOpacity != 1.0) {
            graphicsItem->setOpacity(initialOpacity);
            itemstate->setTargetOpacity(1);
        }
    }
}

void DuiBasicLayoutAnimation::layoutAnimationStarted(DuiItemState *itemstate)
{
    if (!itemstate)
        return;

    start();
    //Since start() calls updateCurrentTime immediately, which in turn can call stop()
    //if the animation finishes immediately, which in turn can end up deleting the object,
    //we cannot rely on itemstate.item() being valid after this point
}

void DuiBasicLayoutAnimation::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    Q_D(DuiBasicLayoutAnimation);
    Q_UNUSED(oldState);
    if (newState == Running)
        d->timeSinceLastUpdate.restart();
}
void DuiBasicLayoutAnimation::updateCurrentTime(int msecs)
{
    Q_D(DuiBasicLayoutAnimation);
    Q_UNUSED(msecs);
    d->tick();
}

