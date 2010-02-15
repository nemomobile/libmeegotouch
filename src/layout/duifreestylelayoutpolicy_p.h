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

#ifndef DUIFREESTYLELAYOUTPOLICY_P_H
#define DUIFREESTYLELAYOUTPOLICY_P_H

#include "duifreestylelayoutpolicy.h"
#include "duiabstractlayoutpolicy_p.h"

/** Private policy class. */
class DuiFreestyleLayoutPolicyPrivate : public DuiAbstractLayoutPolicyPrivate
{
public:
    /** Constructor */
    explicit DuiFreestyleLayoutPolicyPrivate(DuiLayout *);
    /** Destructor */
    virtual ~DuiFreestyleLayoutPolicyPrivate();

protected:
    /** Helper method to move a rectangle away from another one */
    QRectF moveAway(const QRectF &to_move, const QRectF &blocked, const QPointF &topLeft, qreal width) const;
    /** Helper method to place items recursively */
    bool placeItem(int index,
                   QList<QRectF> & placed_items,
                   const QPointF &topLeft,
                   qreal width) const;

private:
    Q_DECLARE_PUBLIC(DuiFreestyleLayoutPolicy)
};

#endif // Header Guard
