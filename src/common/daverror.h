/*
    SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVERROR_H
#define KDAV_DAVERROR_H

#include "kdav_export.h"

#include <KJob>

#include <QSharedDataPointer>
#include <QString>

namespace KDAV
{
/*!
 * DAV operation error codes.
 * \value NO_ERR
 * \value ERR_PROBLEM_WITH_REQUEST
 * \value ERR_NO_MULTIGET
 * \value ERR_SERVER_UNRECOVERABLE
 * \value ERR_COLLECTIONDELETE
 * \value ERR_COLLECTIONFETCH
 * \value ERR_COLLECTIONFETCH_XQUERY_SETFOCUS
 * \value ERR_COLLECTIONFETCH_XQUERY_INVALID
 * \value ERR_COLLECTIONMODIFY
 * \value ERR_COLLECTIONMODIFY_NO_PROPERITES
 * \value ERR_COLLECTIONMODIFY_RESPONSE
 * \value ERR_ITEMCREATE
 * \value ERR_ITEMDELETE
 * \value ERR_ITEMMODIFY
 * \value ERR_ITEMLIST
 * \value ERR_ITEMLIST_NOMIMETYPE
 */
enum ErrorNumber {
    NO_ERR = 0,
    ERR_PROBLEM_WITH_REQUEST = KJob::UserDefinedError + 200, // it would be better to request KIO about uts UserDefinedError space.
    ERR_NO_MULTIGET,
    ERR_SERVER_UNRECOVERABLE,
    ERR_COLLECTIONDELETE = ERR_PROBLEM_WITH_REQUEST + 10,
    ERR_COLLECTIONFETCH = ERR_PROBLEM_WITH_REQUEST + 20,
    ERR_COLLECTIONFETCH_XQUERY_SETFOCUS,
    ERR_COLLECTIONFETCH_XQUERY_INVALID,
    ERR_COLLECTIONMODIFY = ERR_PROBLEM_WITH_REQUEST + 30,
    ERR_COLLECTIONMODIFY_NO_PROPERITES,
    ERR_COLLECTIONMODIFY_RESPONSE,
    ERR_ITEMCREATE = ERR_PROBLEM_WITH_REQUEST + 100,
    ERR_ITEMDELETE = ERR_PROBLEM_WITH_REQUEST + 110,
    ERR_ITEMMODIFY = ERR_PROBLEM_WITH_REQUEST + 120,
    ERR_ITEMLIST = ERR_PROBLEM_WITH_REQUEST + 130,
    ERR_ITEMLIST_NOMIMETYPE,
};

class ErrorPrivate;

/*!
 * \class KDAV::Error
 * \inheaderfile KDAV/DavError
 * \inmodule KDAV
 *
 * \brief DAV operation error.
 */
class KDAV_EXPORT Error
{
public:
    /*!
     *
     */
    explicit Error();

    /*!
     *
     */
    explicit Error(ErrorNumber errNo, int responseCode, const QString &errorText, int jobErrorCode);
    Error(const Error &);
    Error(Error &&);
    ~Error();
    Error &operator=(const Error &);
    Error &operator=(Error &&);

    /*!
     *
     */
    Q_REQUIRED_RESULT ErrorNumber errorNumber() const;

    /*!
     *
     */
    Q_REQUIRED_RESULT int responseCode() const;

    /*!
     *
     */
    Q_REQUIRED_RESULT QString internalErrorText() const;

    /*!
     *
     */
    Q_REQUIRED_RESULT int jobErrorCode() const;

    /*!
     *
     */
    Q_REQUIRED_RESULT QString translatedJobError() const;

    /*!
     *
     */
    Q_REQUIRED_RESULT QString errorText() const;

private:
    QSharedDataPointer<ErrorPrivate> d;
};
}

#endif
