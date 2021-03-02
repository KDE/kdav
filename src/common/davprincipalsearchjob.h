/*
    SPDX-FileCopyrightText: 2011 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVPRINCIPALSEARCHJOB_H
#define KDAV_DAVPRINCIPALSEARCHJOB_H

#include "kdav_export.h"

#include "davjobbase.h"
#include "davurl.h"

#include <QString>

namespace KDAV
{
class DavPrincipalSearchJobPrivate;

/**
 * @class DavPrincipalSearchJob davprincipalsearchjob.h <KDAV/DavPrincipalSearchJob>
 *
 * @short A job that search a DAV principal on a server
 *
 * This job is used to search a principal on a server
 * that implement the dav-property-search REPORT (RFC3744).
 *
 * The properties to fetch are set with @ref fetchProperty().
 */
class KDAV_EXPORT DavPrincipalSearchJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Types of search that are supported by this job.
     * DisplayName will match on the DAV displayname property.
     * EmailAddress will match on the CalDav calendar-user-address-set property.
     */
    enum FilterType {
        DisplayName,
        EmailAddress,
    };

    /**
     * Simple struct to hold the search job results
     */
    struct Result {
        QString propertyNamespace;
        QString property;
        QString value;
    };

    /**
     * Creates a new DAV principal search job
     *
     * @param url The URL to use in the REPORT query.
     * @param type The type that the filter will match.
     * @param filter The filter that will be used to match the displayname attribute.
     * @param parent The parent object.
     */
    explicit DavPrincipalSearchJob(const DavUrl &url, FilterType type, const QString &filter, QObject *parent = nullptr);

    /**
     * Add a new property to fetch from the server.
     *
     * @param name The name of the property.
     * @param ns The namespace of this property, defaults to 'DAV:'.
     */
    void fetchProperty(const QString &name, const QString &ns = QString());

    /**
     * Starts the job
     */
    void start() override;

    /**
     * Return the DavUrl used by this job
     */
    Q_REQUIRED_RESULT DavUrl davUrl() const;

    /**
     * Get the job results.
     */
    Q_REQUIRED_RESULT QVector<Result> results() const;

private:
    Q_DECLARE_PRIVATE(DavPrincipalSearchJob)
};
}

Q_DECLARE_TYPEINFO(KDAV::DavPrincipalSearchJob::Result, Q_MOVABLE_TYPE);
#endif
