/*
    Copyright (c) 2010 Grégory Oestreicher <greg@kamago.net>
    Based on DavItemsListJob which is copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

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

#ifndef KDAV_DAVITEMSFETCHJOB_H
#define KDAV_DAVITEMSFETCHJOB_H

#include "kpimkdav_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

#include <QMap>
#include <QStringList>

namespace KDAV {
class DavItemsFetchJobPrivate;

/**
 * @short A job that fetches a list of items from a DAV server using a multiget query.
 */
class KPIMKDAV_EXPORT DavItemsFetchJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new items fetch job.
     *
     * @param collectionUrl The DAV collection on which to run the query
     * @param urls The list of urls to fetch
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
    void davJobFinished(KJob *);
    Q_DECLARE_PRIVATE(DavItemsFetchJob)
};
}

#endif
