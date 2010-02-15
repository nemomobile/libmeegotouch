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

#ifndef DUINAVIGATIONBAR_H
#define DUINAVIGATIONBAR_H

#include <duiscenewindow.h>
#include <duinavigationbarmodel.h>

class DuiNavigationBarPrivate;
class DuiToolBar;

/*!
 * \class DuiNavigationBar
 * \brief DuiNavigationBar implements a navigation bar with viewmenu
 *
 * DuiNavigationBar doesn't have that many parameters which are read from
 * theme. The theming is done changing the items which lay inside the
 * navigation bar.
 */

class DUI_EXPORT DuiNavigationBar : public DuiSceneWindow
{
    Q_OBJECT
    DUI_CONTROLLER(DuiNavigationBar)

#ifdef UNIT_TEST
    friend class Ut_DuiNavigationBar;
#endif // UNIT_TEST

public:
    /*!
     * \brief Default constructor. Creates a navigation bar.
     */
    DuiNavigationBar();

    /*!
      \brief Destroys the navigation bar.
     */
    virtual ~DuiNavigationBar();

    /**
     * Returns the iconID of the icon of the view menu button.
     */
    QString viewMenuIconID() const;

    /**
     * Returns if the progress indicator is visible
     */
    bool isProgressIndicatorVisible() const;

    /*!
      \brief Returns true if the arrow icon is visible.
    */
    bool isArrowIconVisible() const;

public Q_SLOTS:
    /** \brief Trigger notification of the user via the navigation bar.
     *
     * This slot asks the NavigationBarView to notify the user, e.g.
     * by flashing the Home button.
     */
    void notifyUser();

    /** \brief Dock a toolbar.
     * \param toolbar The toolbar to dock.
     */
    void dockToolBar(DuiToolBar *toolbar);

    /** \brief Undock a toolbar.
     */
    void undockToolBar();

    void setViewMenuDescription(const QString &text);

    /**
     * Sets the new icon of the view menu button.
     * \param the id of the icon that should be shown.
     */
    void setViewMenuIconID(const QString &id);

    /**
     * Sets the visible of progress indicator
     * \param bool visible
     */
    void setProgressIndicatorVisible(bool visible);

    /**
     * Sets the visible of progress indicator
     */
    void setArrowIconVisible(bool visible);

Q_SIGNALS:
    //! \brief viewmenuButton was clicked
    void viewmenuTriggered();

private:
    Q_DISABLE_COPY(DuiNavigationBar)
    Q_DECLARE_PRIVATE(DuiNavigationBar)
};

#endif
