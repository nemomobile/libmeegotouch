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

#ifndef UT_DUILAYOUT_H
#define UT_DUILAYOUT_H

#include <QtTest/QtTest>
#include <QObject>

#include <duilayout.h>

class QGraphicsScene;
class QGraphicsProxyWidget;
class QPushButton;

class Ut_DuiLayout : public QObject
{
    Q_OBJECT

public:
    Ut_DuiLayout();
    virtual ~Ut_DuiLayout();

private slots:
    void cleanupTestCase();
    void init();
    void cleanup();

    void itemState();
    void testTakeAt();
    void testDeletingItem();
    void policyInteraction();
    void animationInteraction();
    void testDeletingLayout();
    void testRemoveItemFromLayout();
    void testEmptyAnimation();
    void testRemovingItemWithAnimation();
    void testReorderingItemsWithoutAnimation();
    void testReorderingItemsWithAnimation();
    void testReorderingItemsWithAnimationWithMultiplePolicies();
    void testDeletingItemWithAnimation();
    void testDeletingLayoutDuringAnimation();
    void testDeletingLayoutDuringAnimationWithMultiplyPolicies();
    void testDeletingThenHiding();
    void testDuiLabelOnLayout();
    void testHidingShowingWidgets_data();
    void testHidingShowingWidgets();
    void testLayoutInsideLayout_data();
    void testLayoutInsideLayout();
    void testReparenting();
    void testReparentingWithPreviousParent();
    void testLinearPolicyChangingOrientation_data();
    void testLinearPolicyChangingOrientation();
    void testGridLayoutShaking();
    void testGridLayoutShaking_data();
    void testLayoutBoundingBox_data();
    void testLayoutBoundingBox();
    void testBasicAnimationWithLayoutInsideLayout();
    void testBasicAnimationWithLayoutInsideLayout2_data();
    void testBasicAnimationWithLayoutInsideLayout2();
    void testQGraphicsLinearLayoutWithLayoutInsideLayout2();
    void testLayoutAddItemWithTwoPolicies_data();
    void testLayoutAddItemWithTwoPolicies();
    void testLayoutRemoveItemWithTwoPolicies_data();
    void testLayoutRemoveItemWithTwoPolicies();
    void testForCrash();
    void testLayoutItemPositionRestoreWhenSwitchingPolicies_data();
    void testLayoutItemPositionRestoreWhenSwitchingPolicies();
    void testLayoutGeometryOfItemsAddedToInactivePolicies_data();
    void testLayoutGeometryOfItemsAddedToInactivePolicies();
    void testLayoutItemOverlap_data();
    void testLayoutItemOverlap();
    void testLayoutPolicyStyling_data();
    void testLayoutPolicyStyling();
    void testChildItems_data();
    void testChildItems();
    void testUserChangingValues();
    void testLayoutSwitchingWithOrientation();
    void testAddingAndRemovingWithSpacing();
    void testLinearPolicyChangingOrientationBasic();
    void testSwitchingPoliciesHidesItems();
    void testSwitchingPoliciesInsidePolicyHidesItems();
    void testAddingRemovingAddingToPolicy();
    void testExplicitlyHidingItems_data();
    void testExplicitlyHidingItems();
private:
    QPushButton *m_button;
    QGraphicsScene *m_scene;
    QGraphicsProxyWidget *m_proxy;
};

#endif // Header guard

