/*
    Copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

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

#ifndef KDAV_DAVMANAGER_H
#define KDAV_DAVMANAGER_H

#include "kpimkdav_export.h"

#include "enums.h"

#include <QtCore/QMap>
#include <QtCore/QString>


class QUrl;

class QDomDocument;
class QWebdav;
class QNetworkAccessManager;

namespace KDAV
{

class DavJob;
class DavProtocolBase;

/**
 * @short A factory class for handling DAV jobs.
 *
 * This class provides factory methods to create preconfigured
 * low-level DAV jobs and has access to the global DAV protocol dialect
 * objects which abstract the access to the various DAV protocol dialects.
 */
class KPIMKDAV_EXPORT DavManager
{
public:
    /**
     * Destroys the DAV manager.
     */
    ~DavManager();

    /**
     * Returns the global instance of the DAV manager.
     */
    static DavManager *self();

    /**
     * Returns a preconfigured DAV PROPFIND job.
     *
     * @param url The target url of the job.
     * @param document The query XML document.
     * @param depth The Depth: value to send in the HTTP request
     */
    DavJob *createPropFindJob(const QUrl &url, const QDomDocument &document, const QString &depth = QStringLiteral("1"));

    /**
     * Returns a preconfigured DAV GET job.
     *
     * @param url The target url of the job.
     */
    DavJob *createGetJob(const QUrl &url);

    /**
     * Returns a preconfigured DAV DELETE job.
     *
     * @param url The target url of the job.
     */
    DavJob *createDeleteJob(const QUrl &url);

    /**
     * Returns a preconfigured DAV PUT job with a If-None-Match header.
     *
     * @param data The data to PUT.
     * @param url The target url of the job.
     * @param contentType The content-type.
     */
    DavJob *createCreateJob(const QByteArray &data, const QUrl &url, const QByteArray &contentType);

    /**
     * Returns a preconfigured DAV PUT job with a If-Match header, that matches the @param etag.
     *
     * @param data The data to PUT.
     * @param url The target url of the job.
     * @param contentType The content-type.
     * @param etag The etag of the entity to modify.
     */
    DavJob *createModifyJob(const QByteArray &data, const QUrl &url, const QByteArray &contentType, const QByteArray &etag);

    /**
     * Returns a preconfigured DAV REPORT job.
     *
     * @param url The target url of the job.
     * @param document The query XML document.
     * @param depth The Depth: value to send in the HTTP request
     */
    DavJob *createReportJob(const QUrl &url, const QDomDocument &document, const QString &depth = QStringLiteral("1"));

    /**
     * Returns a preconfigured DAV PROPPATCH job.
     *
     * @param url The target url of the job.
     * @param document The query XML document.
     */
    DavJob *createPropPatchJob(const QUrl &url, const QDomDocument &document);

    /**
     * Returns the DAV protocol dialect object for the given DAV @p protocol.
     */
    const DavProtocolBase *davProtocol(Protocol protocol);

    /**
     * Provides access to the internally used network access manager.
     */
    static QNetworkAccessManager *networkAccessManager();

    /**
     * Ignore all ssl errors.
     * 
     * If you want to handle ssl errors yourself via the networkAccessManager, then set to false.
     * 
     * Enabled by default.
     */
    void setIgnoreSslErrors(bool);

private:
    /**
     * Creates a new DAV manager.
     */
    DavManager();

    void setConnectionSettings(const QUrl &url);

    /**
     * Creates a new protocol.
     */
    bool createProtocol(Protocol protocol);

    typedef QMap<Protocol, DavProtocolBase *> protocolsMap;
    protocolsMap mProtocols;
    static DavManager *mSelf;
    QWebdav *mWebDav;
    bool mIgnoreSslErrors;
};

}

#endif
