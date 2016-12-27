/*
    Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

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

#ifndef KDAV_DAVERROR_H
#define KDAV_DAVERROR_H

#include "libkdav_export.h"

#include <KJob>

namespace KDAV
{

enum Error {
   ERR_PROBLEM_WITH_REQUEST = KJob::UserDefinedError + 200,         //it would be better to request KIO about uts UserDefinedError space.
   ERR_NO_MULTIGET,
   ERR_SERVER_UNRECOVERABLE,
   ERR_COLLECTIONDELETE,
   ERR_COLLECTIONFETCH,
   ERR_COLLECTIONFETCH_XQUERY_SETFOCUS,
   ERR_COLLECTIONFETCH_XQUERY_INVALID,
   ERR_COLLECTIONMODIFY,
   ERR_COLLECTIONMODIFY_NO_PROPERITES,
   ERR_COLLECTIONMODIFY_RESPONSE,
   ERR_ITEMCREATE,
   ERR_ITEMDELETE,
   ERR_ITEMMODIFY
};

LIBKDAV_EXPORT QString buildErrorString(Error errorCode, const QString &errorText, int responseCode, int jobErrorCode);

}

#endif
