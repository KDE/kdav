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

#include <QMap>
#include <QString>

#include <memory>

namespace KIO {
class DavJob;
}

class QUrl;

class QDomDocument;

/** CalDav/CardDav protocol implementation. */
namespace KDAV {
class DavProtocolBase;
class DavManagerPrivate;

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
    KIO::DavJob *createPropFindJob(const QUrl &url, const QDomDocument &document, const QString &depth = QStringLiteral("1")) const;

    /**
     * Returns a preconfigured DAV REPORT job.
     *
     * @param url The target url of the job.
     * @param document The query XML document.
     * @param depth The Depth: value to send in the HTTP request
     */
    KIO::DavJob *createReportJob(const QUrl &url, const QDomDocument &document, const QString &depth = QStringLiteral("1")) const;

    /**
     * Returns a preconfigured DAV PROPPATCH job.
     *
     * @param url The target url of the job.
     * @param document The query XML document.
     */
    KIO::DavJob *createPropPatchJob(const QUrl &url, const QDomDocument &document) const;

private:
    /**
     * Creates a new DAV manager.
     */
    DavManager();

    friend class DavManagerPrivate;
    const std::unique_ptr<DavManagerPrivate> d;
};
}

#endif
