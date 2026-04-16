/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVMANAGER_P_H
#define KDAV_DAVMANAGER_P_H

#include "enums.h"

#include <QString>

#include <memory>

class QNetworkAccessManager;
class QNetworkReply;
class QUrl;

namespace KDAV
{
class DavProtocolBase;

/*!
 * \internal
 * \brief A factory class for handling DAV jobs.
 *
 * This class provides factory methods to create preconfigured
 * low-level DAV network replies and has access to the global DAV protocol dialect
 * objects which abstract the access to the various DAV protocol dialects.
 */
class DavManager
{
public:
    /*!
     * Destroys the DAV manager.
     */
    ~DavManager();

    /*!
     * Returns the global instance of the DAV manager.
     */
    static DavManager *self();

    /*!
     * Sends a DAV PROPFIND request and returns the network reply.
     *
     * \a url The target URL of the request.
     * \a document The query XML document.
     * \a depth The Depth: value to send in the HTTP request
     */
    QNetworkReply *createPropFindJob(const QUrl &url, const QString &document, const QString &depth = QStringLiteral("1")) const;

    /*!
     * Sends a DAV REPORT request and returns the network reply.
     *
     * \a url The target URL of the request.
     * \a document The query XML document.
     * \a depth The Depth: value to send in the HTTP request
     */
    QNetworkReply *createReportJob(const QUrl &url, const QString &document, const QString &depth = QStringLiteral("1")) const;

    /*!
     * Sends a DAV PROPPATCH request and returns the network reply.
     *
     * \a url The target URL of the request.
     * \a document The query XML document.
     */
    QNetworkReply *createPropPatchJob(const QUrl &url, const QString &document) const;

    /*!
     * Returns the shared QNetworkAccessManager instance.
     */
    QNetworkAccessManager *networkAccessManager() const;

    /*!
     * Returns the user agent.
     */
    QString userAgent() const;

    /*!
     * Returns the DAV protocol dialect object for the given DAV @p protocol.
     */
    static const DavProtocolBase *davProtocol(Protocol protocol);

private:
    /*!
     * Creates a new DAV manager.
     */
    DavManager();

    QNetworkReply *sendDavRequest(const QByteArray &method, const QUrl &url, const QString &document, const QString &depth = {}) const;

    mutable std::unique_ptr<QNetworkAccessManager> mNam;
    std::unique_ptr<DavProtocolBase> mProtocols[3];
};
}

#endif
