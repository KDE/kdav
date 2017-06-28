/*
    Copyright (c) 2010 Grégory Oestreicher <greg@kamago.net>

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

#include "davprincipalhomesetsfetchjob.h"

#include "davmanager.h"
#include "davprotocolbase.h"
#include "daverror.h"
#include "utils.h"
#include "davjob.h"

using namespace KDAV2;

DavPrincipalHomeSetsFetchJob::DavPrincipalHomeSetsFetchJob(const DavUrl &url, QObject *parent)
    : DavJobBase(parent), mUrl(url)
{
}

void DavPrincipalHomeSetsFetchJob::start()
{
    fetchHomeSets(false);
}

void DavPrincipalHomeSetsFetchJob::fetchHomeSets(bool homeSetsOnly)
{
    QDomDocument document;

    QDomElement propfindElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("propfind"));
    document.appendChild(propfindElement);

    QDomElement propElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
    propfindElement.appendChild(propElement);

    const QString homeSet = DavManager::self()->davProtocol(mUrl.protocol())->principalHomeSet();
    const QString homeSetNS = DavManager::self()->davProtocol(mUrl.protocol())->principalHomeSetNS();
    propElement.appendChild(document.createElementNS(homeSetNS, homeSet));

    if (!homeSetsOnly) {
        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("current-user-principal")));
        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("principal-URL")));
    }

    DavJob *job = DavManager::self()->createPropFindJob(mUrl.url(), document, QStringLiteral("0"));
    connect(job, &DavJob::result, this, &DavPrincipalHomeSetsFetchJob::davJobFinished);
}

QStringList DavPrincipalHomeSetsFetchJob::homeSets() const
{
    return mHomeSets;
}

void DavPrincipalHomeSetsFetchJob::davJobFinished(KJob *job)
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

    const QString homeSet = DavManager::self()->davProtocol(mUrl.protocol())->principalHomeSet();
    const QString homeSetNS = DavManager::self()->davProtocol(mUrl.protocol())->principalHomeSetNS();
    QString nextRoundHref; // The content of the href element that will be used if no homeset was found.
    // This is either given by current-user-principal or by principal-URL.

    const QDomDocument document = davJob->response();
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
