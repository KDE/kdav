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

#ifndef KDAV_ENUMS_H
#define KDAV_ENUMS_H

#include <QFlags>

namespace KDAV {
/**
 * Describes the DAV protocol dialect.
 */
enum Protocol {
    CalDav = 0,   ///< The CalDav protocol as defined in https://devguide.calconnect.org/CalDAV
    CardDav,      ///< The CardDav protocol as defined in https://devguide.calconnect.org/CardDAV
    GroupDav,     ///< The GroupDav protocol as defined in http://www.groupdav.org
    PROTOCOL_COUNT
};

/**
 * Describes the DAV privileges on a resource (see RFC3744)
 */
enum Privilege {
    None = 0x0,
    Read = 0x1,
    Write = 0x2,
    WriteProperties = 0x4,
    WriteContent = 0x8,
    Unlock = 0x10,
    ReadAcl = 0x20,
    ReadCurrentUserPrivilegeSet = 0x40,
    WriteAcl = 0x80,
    Bind = 0x100,
    Unbind = 0x200,
    All = 0x400
};
Q_DECLARE_FLAGS(Privileges, Privilege)
Q_DECLARE_OPERATORS_FOR_FLAGS(Privileges)
}

#endif
