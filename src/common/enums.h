/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_ENUMS_H
#define KDAV_ENUMS_H

#include <QFlags>

/**
 * The KDAV namespace.
 */
namespace KDAV
{
/**
 * Describes the DAV protocol dialect.
 */
enum Protocol {
    CalDav = 0, ///< The CalDav protocol as defined in https://devguide.calconnect.org/CalDAV
    CardDav, ///< The CardDav protocol as defined in https://devguide.calconnect.org/CardDAV
    GroupDav, ///< The GroupDav protocol as defined in http://www.groupdav.org
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
    All = 0x400,
};
Q_DECLARE_FLAGS(Privileges, Privilege)
Q_DECLARE_OPERATORS_FOR_FLAGS(Privileges)
}

#endif
