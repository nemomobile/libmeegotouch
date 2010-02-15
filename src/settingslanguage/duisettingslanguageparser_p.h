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

#ifndef DUISETTINGSLANGUAGEPARSER_P_H_
#define DUISETTINGSLANGUAGEPARSER_P_H_

class DuiSettingsLanguageParser;

class DuiSettingsLanguageParserPrivate
{
    Q_DECLARE_PUBLIC(DuiSettingsLanguageParser)

public:
    /*!
     * Constructs a private class for DuiSettingsLanguage.
     */
    DuiSettingsLanguageParserPrivate();

    /*!
     * Destroys the DuiSettingsLanguagePrivate.
     */
    virtual ~DuiSettingsLanguageParserPrivate();

    //! The XML document to parse the information from
    QDomDocument *document;

    /*!
     * Parses an XML element. Basically just selects a parser subroutine.
     * \param item the XML element to parse.
     * \param settingsNode the binary node to which to add the binary nodes
     * generated by the parsing.
     * \return \c true on success, \false on failure.
     */
    bool parse(const QDomElement &item, DuiSettingsLanguageNode &settingsNode);

    /*!
     * Parses a 'settings' element.
     * \param item the XML element to parse.
     * \param settingsNode the binary node to which to add the binary nodes
     * generated by the parsing.
     * \return \c true on success, \false on failure.
     */
    bool parseSettings(const QDomElement &item, DuiSettingsLanguageNode &settingsNode);

    /*!
     * Parses a 'group' element.
     * \param item the XML element to parse.
     * \param settingsNode the binary node to which to add the binary nodes
     * generated by the parsing.
     * \return \c true on success, \false on failure.
     */
    bool parseGroup(const QDomElement &item, DuiSettingsLanguageNode &settingsNode);

    /*!
     * Parses a 'selection' element.
     * \param item the XML element to parse.
     * \param settingsNode the binary node to which to add the binary nodes
     * generated by the parsing.
     * \return \c true on success, \false on failure.
     */
    bool parseSelection(const QDomElement &item, DuiSettingsLanguageNode &settingsNode);

    /*!
     * Parses an 'integer' element.
     * \param item the XML element to parse.
     * \param settingsNode the binary node to which to add the binary nodes
     * generated by the parsing.
     * \return \c true on success, \false on failure.
     */
    bool parseInteger(const QDomElement &item, DuiSettingsLanguageNode &settingsNode);

    /*!
     * Parses an 'option' element.
     * \param item the XML element to parse.
     * \param settingsNode the binary node to which to add the binary nodes
     * generated by the parsing.
     * \return \c true on success, \false on failure.
     */
    bool parseOption(const QDomElement &item, DuiSettingsLanguageNode &settingsNode);

    /*!
     * Parses a 'text' element.
     * \param item the XML element to parse.
     * \param settingsNode the binary node to which to add the binary nodes
     * generated by the parsing.
     * \return \c true on success, \false on failure.
     */
    bool parseText(const QDomElement &item, DuiSettingsLanguageNode &settingsNode);

    /*!
     * Parses a 'boolean' element.
     * \param item the XML element to parse.
     * \param settingsNode the binary node to which to add the binary nodes
     * generated by the parsing.
     * \return \c true on success, \false on failure.
     */
    bool parseBoolean(const QDomElement &item, DuiSettingsLanguageNode &settingsNode);

    /*!
     * Checks existance of attributes in an XML element. Returns \c true
     * if every required attribute is present and \c false otherwise.
     * \param item the XML element to study.
     * \param requiredAttributes a list attributes that are required to be present.
     * \return \c true if attributes are OK, \c false otherwise.
     */
    bool checkAttributes(const QDomElement &item, const QStringList &requiredAttributes) const;

    /*!
     * A helper method for parsing child elements of a DOM element.
     * \param item the parent node whose children will be parsed
     * \param settingsNode the parent settings binary node where the parsed children will be added
     * \param acceptedChildren a list of acceptable child element names
     * \return \c true on success, \false on failure.
     */
    bool parseChildElements(const QDomElement &item, DuiSettingsLanguageNode &settingsNode, const QStringList &acceptedChildren);

protected:
    //! A pointer to the public class
    DuiSettingsLanguageParser *q_ptr;
};

#endif /* DUISETTINGSLANGUAGEPARSER_P_H_ */
