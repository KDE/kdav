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

#include "davprincipalsearchjob.h"

#include "davmanager.h"
#include "utils.h"
#include "daverror.h"
#include "davjob.h"

#include <QtCore/QUrl>

using namespace KDAV2;

DavPrincipalSearchJob::DavPrincipalSearchJob(const DavUrl &url, DavPrincipalSearchJob::FilterType type,
        const QString &filter, QObject *parent)
    : DavJobBase(parent), mUrl(url), mType(type), mFilter(filter), mPrincipalPropertySearchSubJobCount(0),
      mPrincipalPropertySearchSubJobSuccessful(false)
{
}

void DavPrincipalSearchJob::fetchProperty(const QString &name, const QString &ns)
{
    QString propNamespace = ns;
    if (propNamespace.isEmpty()) {
        propNamespace = QStringLiteral("DAV:");
    }

    mFetchProperties << QPair<QString, QString>(propNamespace, name);
}

DavUrl DavPrincipalSearchJob::davUrl() const
{
    return mUrl;
}

void DavPrincipalSearchJob::start()
{
    /*
     * The first step is to try to locate the URL that contains the principals.
     * This is done with a PROPFIND request and a XML like this:
     * <?xml version="1.0" encoding="utf-8" ?>
     * <D:propfind xmlns:D="DAV:">
     *   <D:prop>
     *     <D:principal-collection-set/>
     *   </D:prop>
     * </D:propfind>
     */
    QDomDocument query;

    QDomElement propfind = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("propfind"));
    query.appendChild(propfind);

    QDomElement prop = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
    propfind.appendChild(prop);

    QDomElement principalCollectionSet = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("principal-collection-set"));
    prop.appendChild(principalCollectionSet);

    DavJob *job = DavManager::self()->createPropFindJob(mUrl.url(), query);
    connect(job, &DavJob::result, this, &DavPrincipalSearchJob::principalCollectionSetSearchFinished);
    job->start();
}

void DavPrincipalSearchJob::principalCollectionSetSearchFinished(KJob *job)
{
    DavJob *davJob = qobject_cast<DavJob *>(job);
    const int responseCode = davJob->responseCode();

    if (davJob->error()) {
        setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();

        emitResult();
        return;
    }

    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    /*
     * Extract information from a document like the following:
     *
     * <?xml version="1.0" encoding="utf-8" ?>
     * <D:multistatus xmlns:D="DAV:">
     *   <D:response>
     *     <D:href>http://www.example.com/papers/</D:href>
     *     <D:propstat>
     *       <D:prop>
     *         <D:principal-collection-set>
     *           <D:href>http://www.example.com/acl/users/</D:href>
     *           <D:href>http://www.example.com/acl/groups/</D:href>
     *         </D:principal-collection-set>
     *       </D:prop>
     *       <D:status>HTTP/1.1 200 OK</D:status>
     *     </D:propstat>
     *   </D:response>
     * </D:multistatus>
     */

    QDomDocument document = davJob->response();
    QDomElement documentElement = document.documentElement();

    QDomElement responseElement = Utils::firstChildElementNS(documentElement, QStringLiteral("DAV:"), QStringLiteral("response"));
    if (responseElement.isNull()) {
        emitResult();
        return;
    }

    // check for the valid propstat, without giving up on first error
    QDomElement propstatElement;
    {
        const QDomNodeList propstats = responseElement.elementsByTagNameNS(QStringLiteral("DAV:"), QStringLiteral("propstat"));
        for (int i = 0; i < propstats.length(); ++i) {
            const QDomElement propstatCandidate = propstats.item(i).toElement();
            const QDomElement statusElement = Utils::firstChildElementNS(propstatCandidate, QStringLiteral("DAV:"), QStringLiteral("status"));
            if (statusElement.text().contains(QStringLiteral("200"))) {
                propstatElement = propstatCandidate;
            }
        }
    }

    if (propstatElement.isNull()) {
        emitResult();
        return;
    }

    QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));
    if (propElement.isNull()) {
        emitResult();
        return;
    }

    QDomElement principalCollectionSetElement = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("principal-collection-set"));
    if (principalCollectionSetElement.isNull()) {
        emitResult();
        return;
    }

    QDomNodeList hrefNodes = principalCollectionSetElement.elementsByTagNameNS(QStringLiteral("DAV:"), QStringLiteral("href"));
    for (int i = 0; i < hrefNodes.size(); ++i) {
        QDomElement hrefElement = hrefNodes.at(i).toElement();
        QString href = hrefElement.text();

        QUrl url = mUrl.url();
        if (href.startsWith(QLatin1Char('/'))) {
            // href is only a path, use request url to complete
            url.setPath(href, QUrl::TolerantMode);
        } else {
            // href is a complete url
            QUrl tmpUrl(href);
            tmpUrl.setUserName(url.userName());
            tmpUrl.setPassword(url.password());
            url = tmpUrl;
        }

        QDomDocument principalPropertySearchQuery;
        buildReportQuery(principalPropertySearchQuery);
        DavJob *reportJob = DavManager::self()->createReportJob(url, principalPropertySearchQuery);
        connect(reportJob, &DavJob::result, this, &DavPrincipalSearchJob::principalPropertySearchFinished);
        ++mPrincipalPropertySearchSubJobCount;
        reportJob->start();
    }
}

void DavPrincipalSearchJob::principalPropertySearchFinished(KJob *job)
{
    --mPrincipalPropertySearchSubJobCount;

    if (job->error() && !mPrincipalPropertySearchSubJobSuccessful) {
        setError(job->error());
        setErrorText(job->errorText());
        if (mPrincipalPropertySearchSubJobCount == 0) {
            emitResult();
        }
        return;
    }

    DavJob *davJob = qobject_cast<DavJob *>(job);

    const int responseCode = davJob->responseCode();

    if (responseCode > 499 && responseCode < 600 && !mPrincipalPropertySearchSubJobSuccessful) {
        // Server-side error, unrecoverable
        setLatestResponseCode(responseCode);
        setError(ERR_SERVER_UNRECOVERABLE);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();
        if (mPrincipalPropertySearchSubJobCount == 0) {
            emitResult();
        }
        return;
    } else if (responseCode > 399 && responseCode < 500 && !mPrincipalPropertySearchSubJobSuccessful) {
        setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();

        if (mPrincipalPropertySearchSubJobCount == 0) {
            emitResult();
        }
        return;
    }

    if (!mPrincipalPropertySearchSubJobSuccessful) {
        setError(0);   // nope, everything went fine
        mPrincipalPropertySearchSubJobSuccessful = true;
    }

    /*
     * Extract infos from a document like the following:
     * <?xml version="1.0" encoding="utf-8" ?>
     * <D:multistatus xmlns:D="DAV:" xmlns:B="http://BigCorp.com/ns/">
     *   <D:response>
     *     <D:href>http://www.example.com/users/jdoe</D:href>
     *     <D:propstat>
     *       <D:prop>
     *         <D:displayname>John Doe</D:displayname>
     *       </D:prop>
     *       <D:status>HTTP/1.1 200 OK</D:status>
     *     </D:propstat>
     * </D:multistatus>
    */

    const QDomDocument document = davJob->response();
    const QDomElement documentElement = document.documentElement();

    QDomElement responseElement = Utils::firstChildElementNS(documentElement, QStringLiteral("DAV:"), QStringLiteral("response"));
    if (responseElement.isNull()) {
        if (mPrincipalPropertySearchSubJobCount == 0) {
            emitResult();
        }
        return;
    }

    // check for the valid propstat, without giving up on first error
    QDomElement propstatElement;
    {
        const QDomNodeList propstats = responseElement.elementsByTagNameNS(QStringLiteral("DAV:"), QStringLiteral("propstat"));
        const int propStatsEnd(propstats.length());
        for (int i = 0; i < propStatsEnd; ++i) {
            const QDomElement propstatCandidate = propstats.item(i).toElement();
            const QDomElement statusElement = Utils::firstChildElementNS(propstatCandidate, QStringLiteral("DAV:"), QStringLiteral("status"));
            if (statusElement.text().contains(QStringLiteral("200"))) {
                propstatElement = propstatCandidate;
            }
        }
    }

    if (propstatElement.isNull()) {
        if (mPrincipalPropertySearchSubJobCount == 0) {
            emitResult();
        }
        return;
    }

    QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));
    if (propElement.isNull()) {
        if (mPrincipalPropertySearchSubJobCount == 0) {
            emitResult();
        }
        return;
    }

    // All requested properties are now under propElement, so let's find them
    typedef QPair<QString, QString> PropertyPair;
    foreach (const PropertyPair &fetchProperty, mFetchProperties) {
        QDomNodeList fetchNodes = propElement.elementsByTagNameNS(fetchProperty.first, fetchProperty.second);
        for (int i = 0; i < fetchNodes.size(); ++i) {
            QDomElement fetchElement = fetchNodes.at(i).toElement();
            Result result;
            result.propertyNamespace = fetchProperty.first;
            result.property = fetchProperty.second;
            result.value = fetchElement.text();
            mResults << result;
        }
    }

    if (mPrincipalPropertySearchSubJobCount == 0) {
        emitResult();
    }
}

QList< DavPrincipalSearchJob::Result > DavPrincipalSearchJob::results() const
{
    return mResults;
}

void DavPrincipalSearchJob::buildReportQuery(QDomDocument &query)
{
    /*
     * Build a document like the following, where XXX will
     * be replaced by the properties the user want to fetch:
     *
     *  <?xml version="1.0" encoding="utf-8" ?>
     *  <D:principal-property-search xmlns:D="DAV:">
     *    <D:property-search>
     *      <D:prop>
     *        <D:displayname/>
     *      </D:prop>
     *      <D:match>FILTER</D:match>
     *    </D:property-search>
     *    <D:prop>
     *      XXX
     *    </D:prop>
     *  </D:principal-property-search>
     */

    QDomElement principalPropertySearch = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("principal-property-search"));
    query.appendChild(principalPropertySearch);

    QDomElement propertySearch = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("property-search"));
    principalPropertySearch.appendChild(propertySearch);

    QDomElement prop = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
    propertySearch.appendChild(prop);

    if (mType == DisplayName) {
        QDomElement displayName = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("displayname"));
        prop.appendChild(displayName);
    } else if (mType == EmailAddress) {
        QDomElement calendarUserAddressSet = query.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("calendar-user-address-set"));
        prop.appendChild(calendarUserAddressSet);
        //QDomElement hrefElement = query.createElementNS( "DAV:", "href" );
        //prop.appendChild( hrefElement );
    }

    QDomElement match = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("match"));
    propertySearch.appendChild(match);

    QDomText propFilter = query.createTextNode(mFilter);
    match.appendChild(propFilter);

    prop = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
    principalPropertySearch.appendChild(prop);

    typedef QPair<QString, QString> PropertyPair;
    foreach (const PropertyPair &fetchProperty, mFetchProperties) {
        QDomElement elem = query.createElementNS(fetchProperty.first, fetchProperty.second);
        prop.appendChild(elem);
    }
}
