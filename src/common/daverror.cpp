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
#include <KLocalizedString>

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
    case ERR_PROBLEM_WITH_REQUEST:
        // User-side error
        if (mResponseCode == 401) {
            err = i18n("Invalid username/password");
        } else if (mResponseCode == 403) {
            err = i18n("Access forbidden");
        } else if (mResponseCode == 404) {
            err = i18n("Resource not found");
        } else {
            err = i18n("HTTP error");
        }
        result = i18n("There was a problem with the request.\n"
                      "%1 (%2).", err, mResponseCode);
        break;
    case ERR_NO_MULTIGET:
        result = i18n("Protocol for the collection does not support MULTIGET");
        break;
    case ERR_SERVER_UNRECOVERABLE:
        result = i18n("The server encountered an error that prevented it from completing your request: %1 (%2)", err, mResponseCode);
        break;
    case ERR_COLLECTIONDELETE:
        result = i18n("There was a problem with the request. The collection has not been deleted from the server.\n"
                      "%1 (%2).", err, mResponseCode);
        break;
    case ERR_COLLECTIONFETCH:
        result = i18n("Invalid responses from backend");
        break;
    case ERR_COLLECTIONFETCH_XQUERY_SETFOCUS:
        result = i18n("Error setting focus for XQuery");
        break;
    case ERR_COLLECTIONFETCH_XQUERY_INVALID:
        result = i18n("Invalid XQuery submitted by DAV implementation");
        break;
    case ERR_COLLECTIONMODIFY:
        result = i18n("There was a problem with the request. The collection has not been modified on the server.\n"
                      "%1 (%2).", err, mResponseCode);
        break;
    case ERR_COLLECTIONMODIFY_NO_PROPERITES:
        result = i18n("No properties to change or remove");
        break;
    case ERR_COLLECTIONMODIFY_RESPONSE:
        result = i18n("There was an error when modifying the properties");
        if (!mErrorText.isEmpty()) {
            result.append(i18n("\nThe server returned more information:\n%1", mErrorText));
        }
        break;
    case ERR_ITEMCREATE:
        result = i18n("There was a problem with the request. The item has not been created on the server.\n"
                      "%1 (%2).", err, mResponseCode);
        break;
    case ERR_ITEMDELETE:
        result = i18n("There was a problem with the request. The item has not been deleted from the server.\n"
                      "%1 (%2).", err, mResponseCode);
        break;
    case ERR_ITEMMODIFY:
        result = i18n("There was a problem with the request. The item was not modified on the server.\n"
                      "%1 (%2).", err, mResponseCode);
        break;
    case ERR_ITEMLIST:
    {
        result = i18n("There was a problem with the request.");
        break;
    };
    case ERR_ITEMLIST_NOMIMETYPE:
        result = i18n("There was a problem with the request. The requested MIME types are not supported.");
        break;
    case NO_ERR:
        break;
    }
    return result;
}
