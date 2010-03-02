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

#ifndef DUIAPPLETHANDLEVIEW_P_H_
#define DUIAPPLETHANDLEVIEW_P_H_

#include "duiextensionhandleview_p.h"

class DuiAppletHandleView;

class DuiAppletHandleViewPrivate : public DuiExtensionHandleViewPrivate
{
    Q_DECLARE_PUBLIC(DuiAppletHandleView)

private:
    //! The applet pixmap with the broken state visuals applied
    QPixmap brokenAppletPixmap;

    /*! Convert the given image to black and white. */
    void blackAndWhite(QImage &img);

public:
    DuiAppletHandleViewPrivate(DuiAppletHandle *appletHandle);
    virtual ~DuiAppletHandleViewPrivate();

    //! \reimp
    /*! Shows a dialog for deciding what to do with a broken applet. */
    void showBrokenDialog();

    /*! Shows a dialog for deciding what to do with an installation failed applet. */
    void showInstallationFailedDialog(const QString &error);

    void drawPixmap(QPainter *painter, const QRectF &sourceGeometry, const QRectF &targetGeometry, bool brokenState) const;

    void drawBrokenState();

    //! Deletes the pixmaps
    virtual void destroyPixmaps();
    //! \reimp_end
};

#endif /* DUIAPPLETHANDLEVIEW_P_H_ */
