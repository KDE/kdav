/*
    SPDX-FileCopyrightText: 2011 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davprincipalsearchjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"
#include "utils_p.h"

#include <KIO/DavJob>
#include <KIO/Job>

#include <QUrl>

using namespace KDAV;

namespace KDAV
{
class DavPrincipalSearchJobPrivate : public DavJobBasePrivate
{
public:
    void buildReportQuery(QDomDocument &query) const;
    void principalCollectionSetSearchFinished(KJob *job);
    void principalPropertySearchFinished(KJob *job);

    DavUrl mUrl;
    DavPrincipalSearchJob::FilterType mType;
    QString mFilter;
    int mPrincipalPropertySearchSubJobCount = 0;
    bool mPrincipalPropertySearchSubJobSuccessful = false;
    struct PropertyInfo {
        QString propNS;
        QString propName;
    };
    std::vector<PropertyInfo> mFetchProperties;
    QVector<DavPrincipalSearchJob::Result> mResults;
};
}

DavPrincipalSearchJob::DavPrincipalSearchJob(const DavUrl &url, DavPrincipalSearchJob::FilterType type, const QString &filter, QObject *parent)
    : DavJobBase(new DavPrincipalSearchJobPrivate, parent)
{
    Q_D(DavPrincipalSearchJob);
    d->mUrl = url;
    d->mType = type;
    d->mFilter = filter;
}

void DavPrincipalSearchJob::fetchProperty(const QString &name, const QString &ns)
{
    Q_D(DavPrincipalSearchJob);
    d->mFetchProperties.push_back({!ns.isEmpty() ? ns : QStringLiteral("DAV:"), name});
}

DavUrl DavPrincipalSearchJob::davUrl() const
{
    Q_D(const DavPrincipalSearchJob);
    return d->mUrl;
}

void DavPrincipalSearchJob::start()
{
    Q_D(DavPrincipalSearchJob);
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

    KIO::DavJob *job = DavManager::self()->createPropFindJob(d->mUrl.url(), query.toString());
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    connect(job, &KIO::DavJob::result, this, [d](KJob *job) {
        d->principalCollectionSetSearchFinished(job);
    });
    job->start();
}

void DavPrincipalSearchJobPrivate::principalCollectionSetSearchFinished(KJob *job)
{
    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);
    const QString responseCodeStr = davJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty() ? 0 : responseCodeStr.toInt();
    // KIO::DavJob does not set error() even if the HTTP status code is a 4xx or a 5xx
    if (davJob->error() || (responseCode >= 400 && responseCode < 600)) {
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

    QDomDocument document;
    document.setContent(davJob->responseData(), true);
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
            if (statusElement.text().contains(QLatin1String("200"))) {
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
        KIO::DavJob *reportJob = DavManager::self()->createReportJob(url, principalPropertySearchQuery.toString());
        reportJob->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
        QObject::connect(reportJob, &KIO::DavJob::result, q_ptr, [this](KJob *job) {
            principalPropertySearchFinished(job);
        });
        ++mPrincipalPropertySearchSubJobCount;
        reportJob->start();
    }
}

void DavPrincipalSearchJobPrivate::principalPropertySearchFinished(KJob *job)
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

    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);

    const int responseCode = davJob->queryMetaData(QStringLiteral("responsecode")).toInt();

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
        setError(0); // nope, everything went fine
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

    QDomDocument document;
    document.setContent(davJob->responseData(), true);
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
            if (statusElement.text().contains(QLatin1String("200"))) {
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
    for (const auto &[propNS, propName] : mFetchProperties) {
        const QDomNodeList fetchNodes = propElement.elementsByTagNameNS(propNS, propName);
        mResults.reserve(mResults.size() + fetchNodes.size());
        for (int i = 0; i < fetchNodes.size(); ++i) {
            const QDomElement fetchElement = fetchNodes.at(i).toElement();
            mResults.push_back({propNS, propName, fetchElement.text()});
        }
    }

    if (mPrincipalPropertySearchSubJobCount == 0) {
        emitResult();
    }
}

QVector<DavPrincipalSearchJob::Result> DavPrincipalSearchJob::results() const
{
    Q_D(const DavPrincipalSearchJob);
    return d->mResults;
}

void DavPrincipalSearchJobPrivate::buildReportQuery(QDomDocument &query) const
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

    if (mType == DavPrincipalSearchJob::DisplayName) {
        QDomElement displayName = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("displayname"));
        prop.appendChild(displayName);
    } else if (mType == DavPrincipalSearchJob::EmailAddress) {
        QDomElement calendarUserAddressSet =
            query.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("calendar-user-address-set"));
        prop.appendChild(calendarUserAddressSet);
        // QDomElement hrefElement = query.createElementNS( "DAV:", "href" );
        // prop.appendChild( hrefElement );
    }

    QDomElement match = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("match"));
    propertySearch.appendChild(match);

    QDomText propFilter = query.createTextNode(mFilter);
    match.appendChild(propFilter);

    prop = query.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
    principalPropertySearch.appendChild(prop);

    for (const auto &[propNS, propName] : mFetchProperties) {
        QDomElement elem = query.createElementNS(propNS, propName);
        prop.appendChild(elem);
    }
}
