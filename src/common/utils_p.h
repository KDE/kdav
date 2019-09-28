/*
    Copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef KDAV_DAVUTILS_P_H
#define KDAV_DAVUTILS_P_H

#include <QDomElement>

namespace KDAV {
namespace Utils {
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

