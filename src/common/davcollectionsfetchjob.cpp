/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davcollectionsfetchjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"
#include "davprincipalhomesetsfetchjob.h"
#include "davprotocolbase_p.h"
#include "utils_p.h"

#include "libkdav_debug.h"
#include <KIO/DavJob>
#include <KIO/Job>

#include <QBuffer>
#include <QColor>
#include <QtXmlPatterns/QXmlQuery>

using namespace KDAV;

namespace KDAV
{
class DavCollectionsFetchJobPrivate : public DavJobBasePrivate
{
public:
    void principalFetchFinished(KJob *job);
    void collectionsFetchFinished(KJob *job);
    void doCollectionsFetch(const QUrl &url);
    void subjobFinished();

    DavUrl mUrl;
    DavCollection::List mCollections;
    uint mSubJobCount = 0;

    Q_DECLARE_PUBLIC(DavCollectionsFetchJob)
};
}

DavCollectionsFetchJob::DavCollectionsFetchJob(const DavUrl &url, QObject *parent)
    : DavJobBase(new DavCollectionsFetchJobPrivate, parent)
{
    Q_D(DavCollectionsFetchJob);
    d->mUrl = url;
}

void DavCollectionsFetchJob::start()
{
    Q_D(DavCollectionsFetchJob);
    if (DavManager::davProtocol(d->mUrl.protocol())->supportsPrincipals()) {
        DavPrincipalHomeSetsFetchJob *job = new DavPrincipalHomeSetsFetchJob(d->mUrl);
        connect(job, &DavPrincipalHomeSetsFetchJob::result, this, [d](KJob *job) {
            d->principalFetchFinished(job);
        });
        job->start();
    } else {
        d->doCollectionsFetch(d->mUrl.url());
    }
}

DavCollection::List DavCollectionsFetchJob::collections() const
{
    Q_D(const DavCollectionsFetchJob);
    return d->mCollections;
}

DavUrl DavCollectionsFetchJob::davUrl() const
{
    Q_D(const DavCollectionsFetchJob);
    return d->mUrl;
}

void DavCollectionsFetchJobPrivate::doCollectionsFetch(const QUrl &url)
{
    ++mSubJobCount;

    const QDomDocument collectionQuery = DavManager::davProtocol(mUrl.protocol())->collectionsQuery()->buildQuery();

    KIO::DavJob *job = DavManager::self()->createPropFindJob(url, collectionQuery.toString());
    QObject::connect(job, &KIO::DavJob::result, q_ptr, [this](KJob *job) {
        collectionsFetchFinished(job);
    });
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
}

void DavCollectionsFetchJobPrivate::principalFetchFinished(KJob *job)
{
    const DavPrincipalHomeSetsFetchJob *davJob = qobject_cast<DavPrincipalHomeSetsFetchJob *>(job);

    if (davJob->error()) {
        if (davJob->latestResponseCode()) {
            // If we have a HTTP response code then this may mean that
            // the URL was not a principal URL. Retry as if it were a calendar URL.
            qCDebug(KDAV_LOG) << job->errorText();
            doCollectionsFetch(mUrl.url());
        } else {
            // Just give up here.
            setDavError(davJob->davError());
            setErrorTextFromDavError();
            emitResult();
        }

        return;
    }

    const QStringList homeSets = davJob->homeSets();
    qCDebug(KDAV_LOG) << "Found" << homeSets.size() << "homesets";
    qCDebug(KDAV_LOG) << homeSets;

    if (homeSets.isEmpty()) {
        // Same as above, retry as if it were a calendar URL.
        doCollectionsFetch(mUrl.url());
        return;
    }

    for (const QString &homeSet : homeSets) {
        QUrl url = mUrl.url();

        if (homeSet.startsWith(QLatin1Char('/'))) {
            // homeSet is only a path, use request url to complete
            url.setPath(homeSet, QUrl::TolerantMode);
        } else {
            // homeSet is a complete url
            QUrl tmpUrl(homeSet);
            tmpUrl.setUserName(url.userName());
            tmpUrl.setPassword(url.password());
            url = tmpUrl;
        }

        doCollectionsFetch(url);
    }
}

void DavCollectionsFetchJobPrivate::collectionsFetchFinished(KJob *job)
{
    Q_Q(DavCollectionsFetchJob);
    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);
    const QString responseCodeStr = davJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty() ? 0 : responseCodeStr.toInt();

    // KIO::DavJob does not set error() even if the HTTP status code is a 4xx or a 5xx
    if (davJob->error() || (responseCode >= 400 && responseCode < 600)) {
        if (davJob->url() != mUrl.url()) {
            // Retry as if the initial URL was a calendar URL.
            // We can end up here when retrieving a homeset on
            // which a PROPFIND resulted in an error
            doCollectionsFetch(mUrl.url());
            --mSubJobCount;
            return;
        }

        setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();
    } else {
        // For use in the collectionDiscovered() signal
        QUrl _jobUrl = mUrl.url();
        _jobUrl.setUserInfo(QString());
        const QString jobUrl = _jobUrl.toDisplayString();

        // Validate that we got a valid PROPFIND response
        QDomDocument response;
        response.setContent(davJob->responseData(), true);
        QDomElement rootElement = response.documentElement();
        if (rootElement.tagName().compare(QLatin1String("multistatus"), Qt::CaseInsensitive) != 0) {
            setError(ERR_COLLECTIONFETCH);
            setErrorTextFromDavError();
            subjobFinished();
            return;
        }

        QByteArray resp = davJob->responseData();
        QBuffer buffer(&resp);
        buffer.open(QIODevice::ReadOnly);

        QXmlQuery xquery;
        if (!xquery.setFocus(&buffer)) {
            setError(ERR_COLLECTIONFETCH_XQUERY_SETFOCUS);
            setErrorTextFromDavError();
            subjobFinished();
            return;
        }

        xquery.setQuery(DavManager::davProtocol(mUrl.protocol())->collectionsXQuery());
        if (!xquery.isValid()) {
            setError(ERR_COLLECTIONFETCH_XQUERY_INVALID);
            setErrorTextFromDavError();
            subjobFinished();
            return;
        }

        QString responsesStr;
        xquery.evaluateTo(&responsesStr);
        responsesStr.prepend(QLatin1String("<responses>"));
        responsesStr.append(QLatin1String("</responses>"));

        QDomDocument document;
        if (!document.setContent(responsesStr, true)) {
            setError(ERR_COLLECTIONFETCH);
            setErrorTextFromDavError();
            subjobFinished();
            return;
        }

        if (!q->error()) {
            /*
             * Extract information from a document like the following:
             *
             * <responses>
             *   <response xmlns="DAV:">
             *     <href xmlns="DAV:">/caldav.php/test1.user/home/</href>
             *     <propstat xmlns="DAV:">
             *       <prop xmlns="DAV:">
             *         <C:supported-calendar-component-set xmlns:C="urn:ietf:params:xml:ns:caldav">
             *           <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VEVENT"/>
             *           <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VTODO"/>
             *           <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VJOURNAL"/>
             *           <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VTIMEZONE"/>
             *           <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VFREEBUSY"/>
             *         </C:supported-calendar-component-set>
             *         <resourcetype xmlns="DAV:">
             *           <collection xmlns="DAV:"/>
             *           <C:calendar xmlns:C="urn:ietf:params:xml:ns:caldav"/>
             *           <C:schedule-calendar xmlns:C="urn:ietf:params:xml:ns:caldav"/>
             *         </resourcetype>
             *         <displayname xmlns="DAV:">Test1 User</displayname>
             *         <current-user-privilege-set xmlns="DAV:">
             *           <privilege xmlns="DAV:">
             *             <read xmlns="DAV:"/>
             *           </privilege>
             *         </current-user-privilege-set>
             *         <getctag xmlns="http://calendarserver.org/ns/">12345</getctag>
             *       </prop>
             *       <status xmlns="DAV:">HTTP/1.1 200 OK</status>
             *     </propstat>
             *   </response>
             * </responses>
             */

            const QDomElement responsesElement = document.documentElement();

            QDomElement responseElement = Utils::firstChildElementNS(responsesElement, QStringLiteral("DAV:"), QStringLiteral("response"));
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

                // extract url
                const QDomElement hrefElement = Utils::firstChildElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("href"));
                if (hrefElement.isNull()) {
                    responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
                    continue;
                }

                QString href = hrefElement.text();
                if (!href.endsWith(QLatin1Char('/'))) {
                    href.append(QLatin1Char('/'));
                }

                QUrl url = davJob->url();
                url.setUserInfo(QString());
                if (href.startsWith(QLatin1Char('/'))) {
                    // href is only a path, use request url to complete
                    url.setPath(href, QUrl::TolerantMode);
                } else {
                    // href is a complete url
                    url = QUrl::fromUserInput(href);
                }

                // don't add this resource if it has already been detected
                bool alreadySeen = false;
                for (const DavCollection &seen : std::as_const(mCollections)) {
                    if (seen.url().toDisplayString() == url.toDisplayString()) {
                        alreadySeen = true;
                    }
                }
                if (alreadySeen) {
                    responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
                    continue;
                }

                // extract display name
                const QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));
                const QDomElement displaynameElement = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("displayname"));
                const QString displayName = displaynameElement.text();

                // Extract CTag
                const QDomElement CTagElement = Utils::firstChildElementNS(propElement, //
                                                                           QStringLiteral("http://calendarserver.org/ns/"),
                                                                           QStringLiteral("getctag"));
                QString CTag;
                if (!CTagElement.isNull()) {
                    CTag = CTagElement.text();
                }

                // extract calendar color if provided
                const QDomElement colorElement = Utils::firstChildElementNS(propElement, //
                                                                            QStringLiteral("http://apple.com/ns/ical/"),
                                                                            QStringLiteral("calendar-color"));
                QColor color;
                if (!colorElement.isNull()) {
                    QString colorValue = colorElement.text();
                    if (QColor::isValidColor(colorValue)) {
                        // Color is either #RRGGBBAA or #RRGGBB but QColor expects #AARRGGBB
                        // so we put the AA in front if the string's length is 9.
                        if (colorValue.size() == 9) {
                            QString fixedColorValue = QStringLiteral("#") + colorValue.mid(7, 2) + colorValue.mid(1, 6);
                            color.setNamedColor(fixedColorValue);
                        } else {
                            color.setNamedColor(colorValue);
                        }
                    }
                }

                // extract allowed content types
                const DavCollection::ContentTypes contentTypes = DavManager::davProtocol(mUrl.protocol())->collectionContentTypes(propstatElement);

                auto _url = url;
                _url.setUserInfo(mUrl.url().userInfo());
                DavCollection collection(DavUrl(_url, mUrl.protocol()), displayName, contentTypes);

                collection.setCTag(CTag);
                if (color.isValid()) {
                    collection.setColor(color);
                }

                // extract privileges
                const QDomElement currentPrivsElement = Utils::firstChildElementNS(propElement, //
                                                                                   QStringLiteral("DAV:"),
                                                                                   QStringLiteral("current-user-privilege-set"));
                if (currentPrivsElement.isNull()) {
                    // Assume that we have all privileges
                    collection.setPrivileges(KDAV::All);
                } else {
                    Privileges privileges = Utils::extractPrivileges(currentPrivsElement);
                    collection.setPrivileges(privileges);
                }

                qCDebug(KDAV_LOG) << url.toDisplayString() << "PRIVS: " << collection.privileges();
                mCollections << collection;
                Q_EMIT q->collectionDiscovered(mUrl.protocol(), url.toDisplayString(), jobUrl);

                responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            }
        }
    }

    subjobFinished();
}

void DavCollectionsFetchJobPrivate::subjobFinished()
{
    if (--mSubJobCount == 0) {
        emitResult();
    }
}
