/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_ENUMS_H
#define KDAV_ENUMS_H

#include <QFlags>

/*!
 * \namespace KDAV
 * \inmodule KDAV
 * \inheaderfile KDAV/Enums
 * \brief The KDAV namespace.
 */
namespace KDAV
{
/*!
 * Describes the DAV protocol dialect.
 *
 * \value CalDav The CalDav protocol as defined in https://devguide.calconnect.org/CalDAV
 * \value CardDav The CardDav protocol as defined in https://devguide.calconnect.org/CardDAV
 * \value GroupDav The GroupDav protocol as defined in http://www.groupdav.org
 *
 */
enum Protocol {
    CalDav = 0,
    CardDav,
    GroupDav,
};

/*!
 * Describes the DAV privileges on a resource (see RFC3744)
 *
 * \value None
 * \value Read
 * \value Write
 * \value WriteProperties
 * \value WriteContent
 * \value Unlock
 * \value ReadAcl
 * \value ReadCurrentUserPrivilegeSet
 * \value WriteAcl
 * \value Bind
 * \value Unbind
 * \value All
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
