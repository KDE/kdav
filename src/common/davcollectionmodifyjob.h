/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVCOLLECTIONMODIFYJOB_H
#define KDAV_DAVCOLLECTIONMODIFYJOB_H

#include "kdav_export.h"

#include "davjobbase.h"
#include "davurl.h"

namespace KDAV
{
class DavCollectionModifyJobPrivate;

/**
 * @class DavCollectionModifyJob davcollectionmodifyjob.h <KDAV/DavCollectionModifyJob>
 *
 * @short A job that modifies a DAV collection.
 *
 * This job is used to modify a property of a DAV collection
 * on the DAV server.
 */
class KDAV_EXPORT DavCollectionModifyJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new DAV collection modify job.
     *
     * @param url The DAV URL that identifies the collection.
     * @param parent The parent object.
     */
    explicit DavCollectionModifyJob(const DavUrl &url, QObject *parent = nullptr);

    /**
     * Sets the property that shall be modified by the job.
     *
     * @param property The name of the property.
     * @param value The value of the property.
     * @param ns The XML namespace that shall be used for the property name.
     */
    void setProperty(const QString &property, const QString &value, const QString &ns = QString());

    /**
     * Sets the property that shall be removed by the job.
     *
     * @param property The name of the property.
     * @param ns The XML namespace that shall be used for the property name.
     */
    void removeProperty(const QString &property, const QString &ns);

    /**
     * Starts the job.
     */
    void start() override;

private:
    Q_DECLARE_PRIVATE(DavCollectionModifyJob)
};
}

#endif
