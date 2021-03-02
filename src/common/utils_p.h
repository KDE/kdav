/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVUTILS_P_H
#define KDAV_DAVUTILS_P_H

#include "enums.h"

#include <QDomElement>

namespace KDAV
{
namespace Utils
{
/**
 * Returns the first child element of @p parent that has the given @p tagName and is part of the @p namespaceUri.
 */
Q_REQUIRED_RESULT QDomElement firstChildElementNS(const QDomElement &parent, const QString &namespaceUri, const QString &tagName);

/**
 * Returns the next sibling element of @p element that has the given @p tagName and is part of the @p namespaceUri.
 */
Q_REQUIRED_RESULT QDomElement nextSiblingElementNS(const QDomElement &element, const QString &namespaceUri, const QString &tagName);

/**
 * Extracts privileges from @p element. The <privilege/> tags are expected to be first level children of @p element.
 */
Q_REQUIRED_RESULT Privileges extractPrivileges(const QDomElement &element);

/**
 * Parses a single <privilege/> tag and returns the final Privileges.
 */
Q_REQUIRED_RESULT Privileges parsePrivilege(const QDomElement &element);

}
}

#endif
