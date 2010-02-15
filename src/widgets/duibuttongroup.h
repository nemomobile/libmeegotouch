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

#ifndef DUIBUTTONGROUP_H
#define DUIBUTTONGROUP_H

#include "duiexport.h"

#include <QObject>

class DuiButtonGroupPrivate;
class DuiButton;

/*!
    \class DuiButtonGroup
    \brief DuiButtonGroup organizes groups of button widgets.

    \ingroup widgets

    \section DuiButtonGroupOverview Overview

        DuiButtonGroup does not provide a visual representation of this grouping,
        instead it manages the states of each of the buttons in the group.

        An exclusive button group switches off all checkable (toggle) buttons
        except the one that was clicked. By default, a button group is exclusive.
        The buttons in a button group are usually checkable buttons. Non-checkable
        buttons can be added into group as well but they don't affect the button
        selection policy at all, however the interaction signals are emitted normally
        for them too.

        If you create an exclusive button group, you should ensure that one of
        the buttons in the group is initially checked; otherwise, the group
        will initially be in a state where no buttons are checked.

        A button is added to the group with addButton(). It can be removed from
        the group with removeButton(). If the group is exclusive, the currently
        checked button is available as checkedButton(). If a button is clicked
        the buttonClicked() signal is emitted. For a checkable button in an
        exclusive group this means that the button was checked. The list of
        buttons in the group is returned by buttons().

        In addition, DuiButtonGroup can map between integers and buttons. You
        can assign an integer id to a button with setId(), and retrieve it with
        id(). The id of the currently checked button is available with checkedId(),
        and there is an overloaded signal buttonClicked() which emits the id of
        the button. The purpose of the mapping mechanism is to simplify the
        representation of enum values in a user interface.

    \section DuiButtonGroupUseGuidelines Usage guidelines
        - The button group should have a visualization that ties the buttons in
          the group together.
        - The button group can have a minimum of 2 buttons.
        - The button group component should try to take all the width that is
          available for the group if possible.
        - The button group should automatically wrap into multiple lines if not all
          components fit into one line.

    \section DuiButtonGroupInteractions Interactions
        - Tap down an unselected button
            - Highlights the tapped button, does not change currently selected
              button.
        - Single tap on unselected button
            - Selected state moves from previously selected button to tapped one.
              Performs possible additional action defined by application.
        - Single tap on already selected button
            - Button remains selected, nothing else happens.
        - Drag selected button onto adjacent button
            - Original button remains selected, adjacent button will not get
              selected.

    \section DuiButtonGroupOpenIssues Open issues
        - Should button group be always exclusive?
        - The button group should have a visualization that ties the buttons in
          the group together. How to implement this?
        - Should the group handle the positioning of the buttons?

    \section DuiButtonGroupExamples Examples
        Adding buttons to group:
        \code
        //create button group
        DuiButtonGroup* buttonGroup = new DuiButtonGroup();

        //add buttons to group
        DuiButton* buttons[5];
        for( int i=0; i < 5; ++i) {
            buttons[i] = new DuiButton("Button" + QString::number(i));
            buttons[i]->setViewType(DuiButton::toggleType);
            buttons[i]->setCheckable(true);
            buttonGroup->addButton(buttons[i]);
        }

        //connect to button interaction signals
        connect(buttonGroup, SIGNAL(buttonPressed(DuiButton*)), this, SLOT(groupButtonPressed(DuiButton*)));
        connect(buttonGroup, SIGNAL(buttonReleased(DuiButton*)), this, SLOT(groupButtonReleased(DuiButton*)));
        connect(buttonGroup, SIGNAL(buttonClicked(DuiButton*)), this, SLOT(groupButtonClicked(DuiButton*)));

        //you can add non-checkable buttons in group as well
        DuiButton* button = new DuiButton();
        buttonGroup->addButton(button);

        //and maybe later set them as checkable
        button->setCheckable(true);

        //remove button
        buttonGroup->removeButton(buttons[0]);

        //the group does not free the added buttons they need to be manually deleted by the user
        //when deleting a button it is automatically removed from the group it belongs
        delete button;
        for( int i=0; i<5; ++i)
            delete buttons[i];
        \endcode

        User defined button ids in a group:
        \code
        //create button group
        DuiButtonGroup* buttonGroup = new DuiButtonGroup();

        //add buttons with user define ids to group
        DuiButton* buttons[5];
        for( int i=0; i < 5; ++i) {
            buttons[i] = new DuiButton("Button" + QString::number(i));
            buttons[i]->setViewType(DuiButton::toggleType);
            buttons[i]->setCheckable(true);
            buttonGroup->addButton(buttons[i], i);
        }

        //connect to button interaction signals
        connect(buttonGroup, SIGNAL(buttonPressed(DuiButton*)), this, SLOT(groupButtonPressed(DuiButton*)));
        connect(buttonGroup, SIGNAL(buttonReleased(DuiButton*)), this, SLOT(groupButtonReleased(DuiButton*)));
        connect(buttonGroup, SIGNAL(buttonClicked(DuiButton*)), this, SLOT(groupButtonClicked(DuiButton*)));

        //when giving ids for buttons signals with button ids are emitted by buttongroup in
        //addition of the normal signals, when ids are not assigned these signals are not emitted
        connect(buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(groupButtonPressed(int)));
        connect(buttonGroup, SIGNAL(buttonReleased(int)), this, SLOT(groupButtonReleased(int)));
        connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(groupButtonClicked(int)));

        //fetch button by id
        DuiButton* button = buttonGroup->button(2);

        //fetch id of a button
        int id = buttonGroup->id(button);
        \endcode

    \sa DuiButton
*/

//not properly supported yet
/*
        Multiselection group:
        \code
            //create non-exclusive buttongroup
            DuiButtonGroup* buttonGroup = new DuiButtonGroup();
            buttonGroup->setExclusive(false);
        \endcode
*/

class DUI_EXPORT DuiButtonGroup : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool exclusive READ exclusive WRITE setExclusive)

public:

    /*!
        \brief Constructs a new, empty button group with the given \a parent.
    */
    DuiButtonGroup(QObject *parent = 0);

    /*!
        \brief Destroys the button group.
    */
    virtual ~DuiButtonGroup();

    /*!
        \brief Adds the given \a button to the end of the group's internal list
               of buttons.

         The group does not take the ownership of the button, the caller of the
         method is still responsible for releasing the button.
    */
    void addButton(DuiButton *button);

    /*!
        \brief Adds the given \a button to the button group, with the given
               \a id.

         The group does not take the ownership of the button, the caller of the
         method is still responsible for releasing the button.

         \a id can not be -1.
    */
    void addButton(DuiButton *button, int id);

    /*!
        \brief Returns the button with the specified \a id, or 0 if no such
               button exists.
    */
    DuiButton *button(int id) const;

    /*!
        \brief Returns the list of this group's buttons. This may be empty.
    */
    QList<DuiButton *> buttons() const;

    /*!
        \brief Returns the button group's checked button, or 0 if no buttons
               are checked.
    */
    DuiButton *checkedButton() const;

    /*!
        \brief Returns the id of the checkedButton(), or -1 if no button is
               checked or button without id is checked.
    */
    int checkedId() const;

    /*!
        \brief Returns true if the group is exclusive
     */
    bool exclusive(void) const;

    /*!
        \brief Set whether the button group is exclusive.

        If the group is exclusive, then only one button in the group can be
        checked at any given time. The user can click on any button to check it,
        and that button will replace the existing one as the checked button in
        the group.
    */
    void setExclusive(bool);

    /*!
        \brief Returns the id for the specified \a button, or -1 if no such
               button exists.
    */
    int id(DuiButton *button) const;

    /*!
        \brief Removes the given \a button from the button group.
    */
    void removeButton(DuiButton *button);

    /*!
        \brief Sets the \a id for the specified \a button.

        \a id can not be -1.
    */
    void setId(DuiButton *button, int id);

Q_SIGNALS:

    /*!
        \brief This signal is emitted when the given button is clicked.

        A button is clicked when it is first pressed and then released, or
        programmatically when DuiButton::click() is called.

        \see DuiButton::clicked().
    */
    void buttonClicked(DuiButton *button);

    /*!
        \brief This signal is emitted when a button with the given \a id is
               clicked

               Buttons without id does not emit this signal.
    */
    void buttonClicked(int id);

    /*!
        \brief This signal is emitted when the given \a button is pressed down.
    */
    void buttonPressed(DuiButton *button);

    /*!
        \brief This signal is emitted when a button with the given \a id is
               pressed down.

               Buttons without id does not emit this signal.
    */
    void buttonPressed(int id);

    /*!
        \brief This signal is emitted when the given \a button is released.
    */
    void buttonReleased(DuiButton *button);

    /*!
        \brief This signal is emitted when a button with the given \a id is
               released.

               Buttons without id does not emit this signal.
    */
    void buttonReleased(int id);

protected:
    //! \cond
    DuiButtonGroupPrivate *const d_ptr;
    //! \endcond

private:

    Q_DISABLE_COPY(DuiButtonGroup)
    Q_DECLARE_PRIVATE(DuiButtonGroup)

private:
    //! \cond
    void validateExclusivity();
    //! \endcond

};

#endif
