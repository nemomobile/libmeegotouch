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

#ifndef DUIAPPLETHANDLEVIEW_H
#define DUIAPPLETHANDLEVIEW_H

#include "duiextensionhandleview.h"
#include "duiapplethandle.h"
#include "duiapplethandlemodel.h"
#include "duiapplethandlestyle.h"

class DuiAppletHandleViewPrivate;

//! \internal
/*!
 * DuiAppletHandleView is a view for the DuiAppletHandle that
 * renders the applet's pixmap, shows whether the applet is in
 * a broken state or not and displays a dialog if the applet
 * is broken and the user taps it.
 */
class DuiAppletHandleView : public DuiExtensionHandleView
{
    Q_OBJECT
    DUI_VIEW(DuiAppletHandleModel, DuiAppletHandleStyle)

public:
    /*!
     * Destroyes the DuiAppletHandleView.
     */
    virtual ~DuiAppletHandleView();

protected:
    /*!
     * Constructs a DuiAppletHandleView. Should be used from derived classes.
     *
     * \param dd the private class
     * \param appletHandle the DuiAppletHandle controller to be used
     */
    DuiAppletHandleView(DuiAppletHandleViewPrivate &dd, DuiAppletHandle *appletHandle);

protected slots:
    //! \reimp
    virtual void updateData(const QList<const char *>& modifications);
    //! \reimp_end

    //! \reimp
    virtual void setupModel();
    //! \reimp_end

    /*!
     * \brief A slot for opening applet settings
     */
    void openAppletSettings();

private :
    Q_DISABLE_COPY(DuiAppletHandleView)
    Q_DECLARE_PRIVATE(DuiAppletHandleView)
};
//! \internal_end

#endif // DUIAPPLETHANDLEVIEW_H
