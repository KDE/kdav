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

/*!
 * \class KDAV::DavPrincipalSearchJob
 * \inheaderfile KDAV/DavPrincipalSearchJob
 * \inmodule KDAV
 *
 * \brief A job that search a DAV principal on a server.
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
    /*!
     * Types of search that are supported by this job.
     * \value DisplayName Will match on the DAV displayname property.
     * \value EmailAddress Will match on the CalDav calendar-user-address-set property.
     */
    enum FilterType {
        DisplayName,
        EmailAddress,
    };

    /*!
     * \inmodule KDAV
     * \brief Simple struct to hold the search job results.
     */
    struct Result {
        /*!
         * \variable KDAV::DavPrincipalSearchJob::Result::propertyNamespace
         */
        QString propertyNamespace;
        /*!
         * \variable KDAV::DavPrincipalSearchJob::Result::property
         */
        QString property;
        /*!
         * \variable KDAV::DavPrincipalSearchJob::Result::value
         */
        QString value;
    };

    /*!
     * Creates a new DAV principal search job
     *
     * \a url The URL to use in the REPORT query.
     *
     * \a type The type that the filter will match.
     *
     * \a filter The filter that will be used to match the displayname attribute.
     *
     * \a parent The parent object.
     */
    explicit DavPrincipalSearchJob(const DavUrl &url, FilterType type, const QString &filter, QObject *parent = nullptr);

    /*!
     * Add a new property to fetch from the server.
     *
     * \a name The name of the property.
     *
     * \a ns The namespace of this property, defaults to 'DAV:'.
     */
    void fetchProperty(const QString &name, const QString &ns = QString());

    /*!
     * Starts the job
     */
    void start() override;

    /*!
     * Return the DavUrl used by this job
     */
    Q_REQUIRED_RESULT DavUrl davUrl() const;

    /*!
     * Get the job results.
     */
    Q_REQUIRED_RESULT QList<Result> results() const;

private:
    Q_DECLARE_PRIVATE(DavPrincipalSearchJob)
};
}

Q_DECLARE_TYPEINFO(KDAV::DavPrincipalSearchJob::Result, Q_RELOCATABLE_TYPE);
#endif
