/*
    Copyright (c) 2011 Grégory Oestreicher <greg@kamago.net>

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

#ifndef KDAV2_DAVPRINCIPALSEARCHJOB_H
#define KDAV2_DAVPRINCIPALSEARCHJOB_H

#include "kpimkdav2_export.h"

#include "davjobbase.h"
#include "davurl.h"

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QString>

#include <KCoreAddons/KJob>

class QDomDocument;

namespace KDAV2
{

/**
 * @short A job that search a DAV principal on a server
 *
 * This job is used to search a principal on a server
 * that implement the dav-property-search REPORT (RFC3744).
 *
 * The properties to fetch are set with @ref fetchProperty().
 */
class KPIMKDAV2_EXPORT DavPrincipalSearchJob : public DavJobBase
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
        EmailAddress
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
     * Creates a new dav principal search job
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
    void start() Q_DECL_OVERRIDE;

    /**
     * Return the DavUrl used by this job
     */
    DavUrl davUrl() const;

    /**
     * Get the job results.
     */
    QList<Result> results() const;

private Q_SLOTS:
    void principalCollectionSetSearchFinished(KJob *job);
    void principalPropertySearchFinished(KJob *job);

private:
    void buildReportQuery(QDomDocument &query);

private:
    DavUrl mUrl;
    FilterType mType;
    QString mFilter;
    int mPrincipalPropertySearchSubJobCount;
    bool mPrincipalPropertySearchSubJobSuccessful;
    QList< QPair<QString, QString> > mFetchProperties;
    QList<Result> mResults;
};

}

#endif
