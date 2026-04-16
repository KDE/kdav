// SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "kdav_export.h"

#include "davcollection.h"
#include "davjobbase.h"
#include "davurl.h"

namespace KDAV
{
class DavCollectionCreateJobPrivate;

/*!
 * \class KDAV::DavCollectionCreateJob
 * \inheaderfile KDAV/DavCollectionCreateJob
 * \inmodule KDAV
 *
 * \brief A job to create a DAV collection on the DAV server.
 * \since 6.26.0
 */
class KDAV_EXPORT DavCollectionCreateJob : public DavJobBase
{
    Q_OBJECT

public:
    /*!
     * Creates a new DAV collection create job.
     *
     * \a item The collection that shall be created.
     *
     * \a parent The parent object.
     */
    explicit DavCollectionCreateJob(const DavCollection &collection, QObject *parent = nullptr);

    /*!
     * Starts the job.
     */
    void start() override;

    /*!
     * Returns the created DAV collection including the correct identifier URL
     * and current ETag information.
     */
    Q_REQUIRED_RESULT DavCollection collection() const;

    /*!
     *
     */
    Q_REQUIRED_RESULT QUrl collectionUrl() const;

private:
    Q_DECLARE_PRIVATE(DavCollectionCreateJob)
};
}
