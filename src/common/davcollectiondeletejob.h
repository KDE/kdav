/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVCOLLECTIONDELETEJOB_H
#define KDAV_DAVCOLLECTIONDELETEJOB_H

#include "kdav_export.h"

#include "davjobbase.h"
#include "davurl.h"

namespace KDAV
{
class DavPushDontNotify;
class DavCollectionDeleteJobPrivate;

/*!
 * \class KDAV::DavCollectionDeleteJob
 * \inheaderfile KDAV/DavCollectionDeleteJob
 * \inmodule KDAV
 *
 * \brief A job that deletes a DAV collection.
 *
 * This job is used to delete a DAV collection at a certain URL.
 */
class KDAV_EXPORT DavCollectionDeleteJob : public DavJobBase
{
    Q_OBJECT

public:
    /*!
     * Creates a new DAV collection delete job.
     *
     * \a url The DAV URL of the collection to delete
     *
     * \a parent The parent object.
     */
    explicit DavCollectionDeleteJob(const DavUrl &url, QObject *parent = nullptr);

    /*!
     * Sets Push-Dont-Notify URLs that should be notified.
     * \since 6.29
     */
    void setPushDontNotify(const DavPushDontNotify &dontNotify);

    /*!
     * Gets Push-Dont-Notify Urls.
     * \since 6.29
     */
    [[nodiscard]] DavPushDontNotify pushDontNotify() const;

    /*!
     * Starts the job.
     */
    void start() override;

private:
    Q_DECLARE_PRIVATE(DavCollectionDeleteJob)
};
}

#endif
