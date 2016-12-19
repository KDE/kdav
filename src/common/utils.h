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

#ifndef KDAV_DAVUTILS_H
#define KDAV_DAVUTILS_H

#include "libkdav_export.h"

#include "enums.h"

#include <QtCore/QUrl>
#include <QtXml/QDomElement>
#include <QtCore/QVector>

namespace KDAV
{

/**
 * @short A namespace that contains helper methods for DAV functionality.
 */
namespace Utils
{
/**
 * Returns the untranslated name of the given DAV @p protocol dialect.
 */
QLatin1String protocolName(Protocol protocol);

/**
 * Returns the protocol matching the given name. This is the opposite of
 * Utils::protocolName().
 */
Protocol protocolByName(const QString &name);

/**
 * Returns the first child element of @p parent that has the given @p tagName and is part of the @p namespaceUri.
 */
QDomElement LIBKDAV_EXPORT firstChildElementNS(const QDomElement &parent, const QString &namespaceUri, const QString &tagName);

/**
 * Returns the next sibling element of @p element that has the given @p tagName and is part of the @p namespaceUri.
 */
QDomElement LIBKDAV_EXPORT nextSiblingElementNS(const QDomElement &element, const QString &namespaceUri, const QString &tagName);

/**
 * Extracts privileges from @p element. The <privilege/> tags are expected to be first level children of @p element.
 */
Privileges LIBKDAV_EXPORT extractPrivileges(const QDomElement &element);

/**
 * Parses a single <privilege/> tag and returns the final Privileges.
 */
Privileges LIBKDAV_EXPORT parsePrivilege(const QDomElement &element);

/**
 * Creates a unique identifier that can be used as a file name to upload the dav item
 */
QString LIBKDAV_EXPORT createUniqueId();

/**
 * Returns the mimetype that shall be used for contact DAV resources using @p protocol.
 */
QString LIBKDAV_EXPORT contactsMimeType(Protocol protocol);
}

}

#endif
