/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davprincipalhomesetsfetchjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"
#include "davprotocolbase_p.h"
#include "protocolinfo.h"
#include "utils_p.h"

#include <KIO/DavJob>
#include <KIO/Job>

using namespace KDAV;

namespace KDAV
{
class DavPrincipalHomeSetsFetchJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(KJob *job);
    /**
     * Start the fetch process.
     *
     * There may be two rounds necessary if the first request
     * does not returns the home sets, but only the current-user-principal
     * or the principal-URL. The bool flag is here to prevent requesting
     * those last two on each request, as they are only fetched in
     * the first round.
     *
     * @param fetchHomeSetsOnly If set to true the request will not include
     *        the current-user-principal and principal-URL props.
     */
    void fetchHomeSets(bool fetchHomeSetsOnly);

    DavUrl mUrl;
    QStringList mHomeSets;
};
}

DavPrincipalHomeSetsFetchJob::DavPrincipalHomeSetsFetchJob(const DavUrl &url, QObject *parent)
    : DavJobBase(new DavPrincipalHomeSetsFetchJobPrivate, parent)
{
    Q_D(DavPrincipalHomeSetsFetchJob);
    d->mUrl = url;
}

void DavPrincipalHomeSetsFetchJob::start()
{
    Q_D(DavPrincipalHomeSetsFetchJob);
    d->fetchHomeSets(false);
}

void DavPrincipalHomeSetsFetchJobPrivate::fetchHomeSets(bool homeSetsOnly)
{
    QDomDocument document;

    QDomElement propfindElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("propfind"));
    document.appendChild(propfindElement);

    QDomElement propElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
    propfindElement.appendChild(propElement);

    const QString homeSet = ProtocolInfo::principalHomeSet(mUrl.protocol());
    const QString homeSetNS = ProtocolInfo::principalHomeSetNS(mUrl.protocol());
    propElement.appendChild(document.createElementNS(homeSetNS, homeSet));

    if (!homeSetsOnly) {
        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("current-user-principal")));
        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("principal-URL")));
    }

    KIO::DavJob *job = DavManager::self()->createPropFindJob(mUrl.url(), document.toString(), QStringLiteral("0"));
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    QObject::connect(job, &KIO::DavJob::result, q_ptr, [this](KJob *job) {
        davJobFinished(job);
    });
}

QStringList DavPrincipalHomeSetsFetchJob::homeSets() const
{
    Q_D(const DavPrincipalHomeSetsFetchJob);
    return d->mHomeSets;
}

void DavPrincipalHomeSetsFetchJobPrivate::davJobFinished(KJob *job)
{
    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);
    const QString responseCodeStr = davJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty() ? 0 : responseCodeStr.toInt();

    // KIO::DavJob does not set error() even if the HTTP status code is a 4xx or a 5xx
    if (davJob->error() || (responseCode >= 400 && responseCode < 600)) {
        QString err;
        if (davJob->error() && davJob->error() != KIO::ERR_WORKER_DEFINED) {
            err = KIO::buildErrorString(davJob->error(), davJob->errorText());
        } else {
            err = davJob->errorText();
        }

        setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();

        emitResult();
        return;
    }

    /*
     * Extract information from a document like the following (if no homeset is defined) :
     *
     * <D:multistatus xmlns:D="DAV:">
     *  <D:response xmlns:D="DAV:">
     *   <D:href xmlns:D="DAV:">/dav/</D:href>
     *   <D:propstat xmlns:D="DAV:">
     *    <D:status xmlns:D="DAV:">HTTP/1.1 200 OK</D:status>
     *    <D:prop xmlns:D="DAV:">
     *     <D:current-user-principal xmlns:D="DAV:">
     *      <D:href xmlns:D="DAV:">/principals/users/gdacoin/</D:href>
     *     </D:current-user-principal>
     *    </D:prop>
     *   </D:propstat>
     *   <D:propstat xmlns:D="DAV:">
     *    <D:status xmlns:D="DAV:">HTTP/1.1 404 Not Found</D:status>
     *    <D:prop xmlns:D="DAV:">
     *     <principal-URL xmlns="DAV:"/>
     *     <calendar-home-set xmlns="urn:ietf:params:xml:ns:caldav"/>
     *    </D:prop>
     *   </D:propstat>
     *  </D:response>
     * </D:multistatus>
     *
     * Or like this (if the homeset is defined):
     *
     *  <?xml version="1.0" encoding="utf-8" ?>
     *  <multistatus xmlns="DAV:" xmlns:C="urn:ietf:params:xml:ns:caldav">
     *    <response>
     *      <href>/principals/users/greg%40kamago.net/</href>
     *      <propstat>
     *        <prop>
     *          <C:calendar-home-set>
     *            <href>/greg%40kamago.net/</href>
     *          </C:calendar-home-set>
     *        </prop>
     *        <status>HTTP/1.1 200 OK</status>
     *      </propstat>
     *    </response>
     *  </multistatus>
     */

    const QString homeSet = ProtocolInfo::principalHomeSet(mUrl.protocol());
    const QString homeSetNS = ProtocolInfo::principalHomeSetNS(mUrl.protocol());
    QString nextRoundHref; // The content of the href element that will be used if no homeset was found.
    // This is either given by current-user-principal or by principal-URL.

    QDomDocument document;
    document.setContent(davJob->responseData(), true);
    const QDomElement multistatusElement = document.documentElement();

    QDomElement responseElement = Utils::firstChildElementNS(multistatusElement, QStringLiteral("DAV:"), QStringLiteral("response"));
    while (!responseElement.isNull()) {
        QDomElement propstatElement;

        // check for the valid propstat, without giving up on first error
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
            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            continue;
        }

        // extract home sets
        const QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));
        const QDomElement homeSetElement = Utils::firstChildElementNS(propElement, homeSetNS, homeSet);

        if (!homeSetElement.isNull()) {
            QDomElement hrefElement = Utils::firstChildElementNS(homeSetElement, QStringLiteral("DAV:"), QStringLiteral("href"));

            while (!hrefElement.isNull()) {
                const QString href = hrefElement.text();
                if (!mHomeSets.contains(href)) {
                    mHomeSets << href;
                }

                hrefElement = Utils::nextSiblingElementNS(hrefElement, QStringLiteral("DAV:"), QStringLiteral("href"));
            }
        } else {
            // Trying to get the principal url, given either by current-user-principal or principal-URL
            QDomElement urlHolder = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("current-user-principal"));
            if (urlHolder.isNull()) {
                urlHolder = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("principal-URL"));
            }

            if (!urlHolder.isNull()) {
                // Getting the href that will be used for the next round
                QDomElement hrefElement = Utils::firstChildElementNS(urlHolder, QStringLiteral("DAV:"), QStringLiteral("href"));
                if (!hrefElement.isNull()) {
                    nextRoundHref = hrefElement.text();
                }
            }
        }

        responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
    }

    /*
     * Now either we got one or more homesets, or we got an href for the next round
     * or nothing can be found by this job.
     * If we have homesets, we're done here and can notify the caller.
     * Else we must ensure that we have an href for the next round.
     */
    if (!mHomeSets.isEmpty() || nextRoundHref.isEmpty()) {
        emitResult();
    } else {
        QUrl nextRoundUrl(mUrl.url());

        if (nextRoundHref.startsWith(QLatin1Char('/'))) {
            // nextRoundHref is only a path, use request url to complete
            nextRoundUrl.setPath(nextRoundHref, QUrl::TolerantMode);
        } else {
            // href is a complete url
            nextRoundUrl = QUrl::fromUserInput(nextRoundHref);
            nextRoundUrl.setUserName(mUrl.url().userName());
            nextRoundUrl.setPassword(mUrl.url().password());
        }

        mUrl.setUrl(nextRoundUrl);
        // And one more round, fetching only homesets
        fetchHomeSets(true);
    }
}
