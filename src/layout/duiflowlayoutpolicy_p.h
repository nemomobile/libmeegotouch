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

#ifndef DUIFLOWLAYOUTPOLICY_P_H
#define DUIFLOWLAYOUTPOLICY_P_H

#include "duiflowlayoutpolicy.h"
#include "duiabstractlayoutpolicy_p.h"

/** Implementation class for DuiFlowLayoutPolicy.
  *
  * As the flow layout policy currently does not need any private data, this
  * class does nothing and exists purely to keep the inheritance trees of
  * private and public classes in sync.
  */
class DuiFlowLayoutPolicyPrivate : public DuiAbstractLayoutPolicyPrivate
{
    Q_DECLARE_PUBLIC(DuiFlowLayoutPolicy)

public:
    /** Constructor */
    explicit DuiFlowLayoutPolicyPrivate(DuiLayout *l);
    /** Destructor */
    virtual ~DuiFlowLayoutPolicyPrivate();

    /** Modify the position and row_height parameters accordingly to place the next item, with the given
     *  current position, item size and constraint size, as long as the item can fit on the current row.
     *  If the item does not fit on the current row, position and row_height are not changed.
     *  @p position must not be NULL.  @row_height can be NULL
     *  @return True if the item fits on this row, False otherwise
     */
    bool roomForItemOnCurrentRow(const QSizeF &item_size, const qreal &constraint_width, QPointF *position, qreal *row_height = NULL) const;

    /** Number of rows that the items occupy.  -1 if not known (because we need to call relayout() first) */
    int numberOfRows;

    /** Maximum number of rows of items to show.  -1 if no limit */
    int rowLimit;
};

#endif // Header Guard
