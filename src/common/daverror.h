/*
    Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef KDAV_DAVERROR_H
#define KDAV_DAVERROR_H

#include "kdav_export.h"

#include <KJob>

#include <QSharedDataPointer>
#include <QString>

namespace KDAV {
/** DAV operation error codes. */
enum ErrorNumber {
    NO_ERR = 0,
    ERR_PROBLEM_WITH_REQUEST = KJob::UserDefinedError + 200,        //it would be better to request KIO about uts UserDefinedError space.
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

class ErrorPrivate;

/** DAV operation error. */
class KDAV_EXPORT Error
{
public:
    explicit Error();
    explicit Error(ErrorNumber errNo, int responseCode, const QString &errorText, int jobErrorCode);
    Error(const Error&);
    ~Error();
    Error& operator=(const Error&);

    Q_REQUIRED_RESULT ErrorNumber errorNumber() const;
    Q_REQUIRED_RESULT int responseCode() const;
    Q_REQUIRED_RESULT QString internalErrorText() const;
    Q_REQUIRED_RESULT int jobErrorCode() const;
    Q_REQUIRED_RESULT QString translatedJobError() const;
    Q_REQUIRED_RESULT QString errorText() const;

private:
    QSharedDataPointer<ErrorPrivate> d;
};
}

#endif
