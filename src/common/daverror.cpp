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

#include "daverror.h"

#include <KIO/Global>
#include <KIO/Job>

using namespace KDAV;

Error::Error()
    : mErrorNumber(NO_ERR)
    , mResponseCode(0)
    , mJobErrorCode(0)
{
}

Error::Error(ErrorNumber errNo, int responseCode, const QString &errorText, int jobErrorCode)
    : mErrorNumber(errNo)
    , mResponseCode(responseCode)
    , mErrorText(errorText)
    , mJobErrorCode(jobErrorCode)
{
}

ErrorNumber Error::errorNumber() const
{
    return mErrorNumber;
}


QString Error::internalErrorText() const
{
    return mErrorText;
}


int Error::jobErrorCode() const
{
    return mJobErrorCode;
}


int Error::responseCode() const
{
    return mResponseCode;
}

QString KDAV::Error::translatedJobError() const
{
    QString err;
    if (mJobErrorCode > 0 && mJobErrorCode != KIO::ERR_SLAVE_DEFINED) {
        err = KIO::buildErrorString(mJobErrorCode, mErrorText);
    } else {
        err = mErrorText;
    }
    return err;
}

QString Error::errorText() const
{
    QString result;

    QString err = translatedJobError();

    switch (mErrorNumber) {
    case ERR_PROBLEM_WITH_REQUEST: {
        // User-side error
        if (mResponseCode == 401) {
            err = QStringLiteral("Invalid username/password");
        } else if (mResponseCode == 403) {
            err = QStringLiteral("Access forbidden");
        } else if (mResponseCode == 404) {
            err = QStringLiteral("Resource not found");
        } else {
            err = QStringLiteral("HTTP error");
        }
        result = QStringLiteral("There was a problem with the request.\n"
                          "%1 (%2).").arg(err, mResponseCode);
        break;
        }
    case ERR_NO_MULTIGET: {
        result = QStringLiteral("Protocol for the collection does not support MULTIGET");
        break;
        }
    case ERR_SERVER_UNRECOVERABLE: {
        result = QStringLiteral("The server encountered an error that prevented it from completing your request: %1 (%2)").arg(err, mResponseCode);
        break;
        }
    case ERR_COLLECTIONDELETE: {
        result = QStringLiteral("There was a problem with the request. The collection has not been deleted from the server.\n"
                          "%1 (%2).").arg(err, mResponseCode);
        break;
        }
    case ERR_COLLECTIONFETCH: {
        result = QStringLiteral("Invalid responses from backend");
        break;
        }
    case ERR_COLLECTIONFETCH_XQUERY_SETFOCUS: {
        result = QStringLiteral("Error setting focus for XQuery");
        break;
        }
    case ERR_COLLECTIONFETCH_XQUERY_INVALID: {
        result = QStringLiteral("Invalid XQuery submitted by DAV implementation");
        break;
        }
    case ERR_COLLECTIONMODIFY: {
        result = QStringLiteral("There was a problem with the request. The collection has not been modified on the server.\n"
                      "%1 (%2).").arg(err, mResponseCode);
        break;
        }
    case ERR_COLLECTIONMODIFY_NO_PROPERITES: {
        result = QStringLiteral("No properties to change or remove");
        break;
        }
    case ERR_COLLECTIONMODIFY_RESPONSE: {
        result = QStringLiteral("There was an error when modifying the properties");
        if (!mErrorText.isEmpty()) {
            result.append(QStringLiteral("\nThe server returned more information:\n%1").arg(mErrorText));
        }
        break;
        }
    case ERR_ITEMCREATE: {
        result = QStringLiteral("There was a problem with the request. The item has not been created on the server.\n"
                      "%1 (%2).").arg(err, mResponseCode);
        break;
        }
    case ERR_ITEMDELETE: {
        result = QStringLiteral("There was a problem with the request. The item has not been deleted from the server.\n"
                      "%1 (%2).").arg(err, mResponseCode);
        break;
        }
    case ERR_ITEMMODIFY: {
        result = QStringLiteral("There was a problem with the request. The item was not modified on the server.\n"
                      "%1 (%2).").arg(err, mResponseCode);
        break;
        }
    case NO_ERR:
        break;
    }
    return result;
}
