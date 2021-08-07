/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVMANAGER_P_H
#define KDAV_DAVMANAGER_P_H

#include "enums.h"

#include <QString>

#include <memory>

namespace KIO
{
class DavJob;
}

class QUrl;

/** CalDav/CardDav protocol implementation. */
namespace KDAV
{
class DavProtocolBase;

/**
 * @short A factory class for handling DAV jobs.
 *
 * This class provides factory methods to create preconfigured
 * low-level DAV jobs and has access to the global DAV protocol dialect
 * objects which abstract the access to the various DAV protocol dialects.
 */
class DavManager
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
     * @param url The target URL of the job.
     * @param document The query XML document.
     * @param depth The Depth: value to send in the HTTP request
     */
    KIO::DavJob *createPropFindJob(const QUrl &url, const QString &document, const QString &depth = QStringLiteral("1")) const;

    /**
     * Returns a preconfigured DAV REPORT job.
     *
     * @param url The target URL of the job.
     * @param document The query XML document.
     * @param depth The Depth: value to send in the HTTP request
     */
    KIO::DavJob *createReportJob(const QUrl &url, const QString &document, const QString &depth = QStringLiteral("1")) const;

    /**
     * Returns a preconfigured DAV PROPPATCH job.
     *
     * @param url The target URL of the job.
     * @param document The query XML document.
     */
    KIO::DavJob *createPropPatchJob(const QUrl &url, const QString &document) const;

    /**
     * Returns the DAV protocol dialect object for the given DAV @p protocol.
     */
    static const DavProtocolBase *davProtocol(Protocol protocol);

private:
    /**
     * Creates a new DAV manager.
     */
    DavManager();

    std::unique_ptr<DavProtocolBase> mProtocols[3];
};
}

#endif
