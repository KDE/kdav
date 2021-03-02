/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    Based on DavItemsListJob:
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVITEMSFETCHJOB_H
#define KDAV_DAVITEMSFETCHJOB_H

#include "kdav_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

#include <QStringList>

namespace KDAV
{
class DavItemsFetchJobPrivate;

/**
 * @class DavItemsFetchJob davitemsfetchjob.h <KDAV/DavItemsFetchJob>
 *
 * @short A job that fetches a list of items from a DAV server using a MULTIGET query.
 */
class KDAV_EXPORT DavItemsFetchJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new items fetch job.
     *
     * @param collectionUrl The DAV collection on which to run the query
     * @param urls The list of URLs to fetch
     * @param parent The parent object
     */
    DavItemsFetchJob(const DavUrl &collectionUrl, const QStringList &urls, QObject *parent = nullptr);

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the list of fetched items
     */
    Q_REQUIRED_RESULT DavItem::List items() const;

    /**
     * Return the item found at @p url
     */
    Q_REQUIRED_RESULT DavItem item(const QString &url) const;

private:
    Q_DECLARE_PRIVATE(DavItemsFetchJob)
};
}

#endif
