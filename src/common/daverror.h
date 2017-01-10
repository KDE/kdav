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

#include "kpimkdav_export.h"

#include <KJob>

#include <QString>

namespace KDAV
{

enum ErrorNumber {
   NO_ERR = 0,
   ERR_PROBLEM_WITH_REQUEST = KJob::UserDefinedError + 200,         //it would be better to request KIO about uts UserDefinedError space.
   ERR_NO_MULTIGET,
   ERR_SERVER_UNRECOVERABLE,
   ERR_COLLECTIONDELETE = ERR_PROBLEM_WITH_REQUEST + 10,
   ERR_COLLECTIONFETCH = ERR_PROBLEM_WITH_REQUEST  + 20,
   ERR_COLLECTIONFETCH_XQUERY_SETFOCUS,
   ERR_COLLECTIONFETCH_XQUERY_INVALID,
   ERR_COLLECTIONMODIFY = ERR_PROBLEM_WITH_REQUEST + 30,
   ERR_COLLECTIONMODIFY_NO_PROPERITES,
   ERR_COLLECTIONMODIFY_RESPONSE,
   ERR_ITEMCREATE = ERR_PROBLEM_WITH_REQUEST + 100,
   ERR_ITEMDELETE = ERR_PROBLEM_WITH_REQUEST + 110,
   ERR_ITEMMODIFY = ERR_PROBLEM_WITH_REQUEST + 120,
   ERR_ITEMLIST = ERR_PROBLEM_WITH_REQUEST + 130,
   ERR_ITEMLIST_NOMIMETYPE
};

class KPIMKDAV_EXPORT Error {
public:
    explicit Error();
    explicit Error(ErrorNumber errNo, int responseCode, const QString &errorText, int jobErrorCode);

    ErrorNumber errorNumber() const;
    int responseCode() const;
    QString internalErrorText() const;
    int jobErrorCode() const;
    QString translatedJobError() const;
    QString errorText() const;

private:
    ErrorNumber mErrorNumber;
    int mResponseCode;
    QString mErrorText;
    int mJobErrorCode;
};

}

#endif
