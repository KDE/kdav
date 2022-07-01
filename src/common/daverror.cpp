/*
    SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "daverror.h"
#include <KLocalizedString>

#include <KIO/Global>
#include <KIO/Job>

using namespace KDAV;

namespace KDAV
{
class ErrorPrivate : public QSharedData
{
public:
    ErrorNumber mErrorNumber = NO_ERR;
    int mResponseCode = 0;
    int mJobErrorCode = 0;
    QString mErrorText;
};
}

Error::Error()
    : d(new ErrorPrivate)
{
}

Error::Error(ErrorNumber errNo, int responseCode, const QString &errorText, int jobErrorCode)
    : d(new ErrorPrivate)
{
    d->mErrorNumber = errNo;
    d->mResponseCode = responseCode;
    d->mErrorText = errorText;
    d->mJobErrorCode = jobErrorCode;
}

Error::Error(const Error &) = default;
Error::Error(Error &&) = default;
Error::~Error() = default;
Error &Error::operator=(const Error &) = default;
Error &Error::operator=(Error &&) = default;

ErrorNumber Error::errorNumber() const
{
    return d->mErrorNumber;
}

QString Error::internalErrorText() const
{
    return d->mErrorText;
}

int Error::jobErrorCode() const
{
    return d->mJobErrorCode;
}

int Error::responseCode() const
{
    return d->mResponseCode;
}

QString KDAV::Error::translatedJobError() const
{
    QString err;
    if (d->mJobErrorCode > 0 && d->mJobErrorCode != KIO::ERR_WORKER_DEFINED) {
        err = KIO::buildErrorString(d->mJobErrorCode, d->mErrorText);
    } else {
        err = d->mErrorText;
    }
    return err;
}

QString Error::errorText() const
{
    QString result;

    QString err = translatedJobError();

    switch (d->mErrorNumber) {
    case ERR_PROBLEM_WITH_REQUEST:
        // User-side error
        if (d->mResponseCode == 401) {
            err = i18n("Invalid username/password");
        } else if (d->mResponseCode == 403) {
            err = i18n("Access forbidden");
        } else if (d->mResponseCode == 404) {
            err = i18n("Resource not found");
        } else {
            err = i18n("HTTP error");
        }
        result = i18n("There was a problem with the request.\n%1 (%2).", err, d->mResponseCode);
        break;
    case ERR_NO_MULTIGET:
        result = i18n("Protocol for the collection does not support MULTIGET");
        break;
    case ERR_SERVER_UNRECOVERABLE:
        result = i18n("The server encountered an error that prevented it from completing your request: %1 (%2)", err, d->mResponseCode);
        break;
    case ERR_COLLECTIONDELETE:
        result = i18n("There was a problem with the request. The collection has not been deleted from the server.\n%1 (%2).", err, d->mResponseCode);
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
        result = i18n(
            "There was a problem with the request. The collection has not been modified on the server.\n"
            "%1 (%2).",
            err,
            d->mResponseCode);
        break;
    case ERR_COLLECTIONMODIFY_NO_PROPERITES:
        result = i18n("No properties to change or remove");
        break;
    case ERR_COLLECTIONMODIFY_RESPONSE:
        result = i18n("There was an error when modifying the properties");
        if (!d->mErrorText.isEmpty()) {
            result.append(i18n("\nThe server returned more information:\n%1", d->mErrorText));
        }
        break;
    case ERR_ITEMCREATE:
        result = i18n("There was a problem with the request. The item has not been created on the server.\n%1 (%2).", err, d->mResponseCode);
        break;
    case ERR_ITEMDELETE:
        result = i18n("There was a problem with the request. The item has not been deleted from the server.\n%1 (%2).", err, d->mResponseCode);
        break;
    case ERR_ITEMMODIFY:
        result = i18n("There was a problem with the request. The item was not modified on the server.\n%1 (%2).", err, d->mResponseCode);
        break;
    case ERR_ITEMLIST: {
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
