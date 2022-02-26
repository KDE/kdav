/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemslistjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"
#include "davprotocolbase_p.h"
#include "davurl.h"
#include "etagcache.h"
#include "utils_p.h"

#include <KIO/DavJob>
#include <KIO/Job>

#include <set>

using namespace KDAV;

namespace KDAV
{
class DavItemsListJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(KJob *job);

    DavUrl mUrl;
    std::shared_ptr<EtagCache> mEtagCache;
    QStringList mMimeTypes;
    QString mRangeStart;
    QString mRangeEnd;
    DavItem::List mItems;
    std::set<QString> mSeenUrls; // to prevent events duplication with some servers
    DavItem::List mChangedItems;
    QStringList mDeletedItems;
    uint mSubJobCount = 0;
};
}

DavItemsListJob::DavItemsListJob(const DavUrl &url, const std::shared_ptr<EtagCache> &cache, QObject *parent)
    : DavJobBase(new DavItemsListJobPrivate, parent)
{
    Q_D(DavItemsListJob);
    d->mUrl = url;
    d->mEtagCache = cache;
}

DavItemsListJob::~DavItemsListJob() = default;

void DavItemsListJob::setContentMimeTypes(const QStringList &types)
{
    Q_D(DavItemsListJob);
    d->mMimeTypes = types;
}

void DavItemsListJob::setTimeRange(const QString &start, const QString &end)
{
    Q_D(DavItemsListJob);
    d->mRangeStart = start;
    d->mRangeEnd = end;
}

void DavItemsListJob::start()
{
    Q_D(DavItemsListJob);
    const DavProtocolBase *protocol = DavManager::davProtocol(d->mUrl.protocol());
    Q_ASSERT(protocol);

    const auto queries = protocol->itemsQueries();
    for (XMLQueryBuilder::Ptr builder : queries) {
        if (!d->mRangeStart.isEmpty()) {
            builder->setParameter(QStringLiteral("start"), d->mRangeStart);
        }
        if (!d->mRangeEnd.isEmpty()) {
            builder->setParameter(QStringLiteral("end"), d->mRangeEnd);
        }

        const QDomDocument props = builder->buildQuery();
        const QString mimeType = builder->mimeType();

        if (d->mMimeTypes.isEmpty() || d->mMimeTypes.contains(mimeType)) {
            ++d->mSubJobCount;
            if (protocol->useReport()) {
                KIO::DavJob *job = DavManager::self()->createReportJob(d->mUrl.url(), props.toString());
                job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
                job->setProperty("davType", QStringLiteral("report"));
                job->setProperty("itemsMimeType", mimeType);
                connect(job, &KIO::DavJob::result, this, [d](KJob *job) {
                    d->davJobFinished(job);
                });
            } else {
                KIO::DavJob *job = DavManager::self()->createPropFindJob(d->mUrl.url(), props.toString());
                job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
                job->setProperty("davType", QStringLiteral("propFind"));
                job->setProperty("itemsMimeType", mimeType);
                connect(job, &KIO::DavJob::result, this, [d](KJob *job) {
                    d->davJobFinished(job);
                });
            }
        }
    }

    if (d->mSubJobCount == 0) {
        setError(ERR_ITEMLIST_NOMIMETYPE);
        d->setErrorTextFromDavError();
        emitResult();
    }
}

DavItem::List DavItemsListJob::items() const
{
    Q_D(const DavItemsListJob);
    return d->mItems;
}

DavItem::List DavItemsListJob::changedItems() const
{
    Q_D(const DavItemsListJob);
    return d->mChangedItems;
}

QStringList DavItemsListJob::deletedItems() const
{
    Q_D(const DavItemsListJob);
    return d->mDeletedItems;
}

void DavItemsListJobPrivate::davJobFinished(KJob *job)
{
    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);
    const int responseCode = davJob->queryMetaData(QStringLiteral("responsecode")).isEmpty() //
        ? 0
        : davJob->queryMetaData(QStringLiteral("responsecode")).toInt();

    // KIO::DavJob does not set error() even if the HTTP status code is a 4xx or a 5xx
    if (davJob->error() || (responseCode >= 400 && responseCode < 600)) {
        setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();
    } else {
        /*
         * Extract data from a document like the following:
         *
         * <multistatus xmlns="DAV:">
         *   <response xmlns="DAV:">
         *     <href xmlns="DAV:">/caldav.php/test1.user/home/KOrganizer-166749289.780.ics</href>
         *     <propstat xmlns="DAV:">
         *       <prop xmlns="DAV:">
         *         <getetag xmlns="DAV:">"b4bbea0278f4f63854c4167a7656024a"</getetag>
         *       </prop>
         *       <status xmlns="DAV:">HTTP/1.1 200 OK</status>
         *     </propstat>
         *   </response>
         *   <response xmlns="DAV:">
         *     <href xmlns="DAV:">/caldav.php/test1.user/home/KOrganizer-399416366.464.ics</href>
         *     <propstat xmlns="DAV:">
         *       <prop xmlns="DAV:">
         *         <getetag xmlns="DAV:">"52eb129018398a7da4f435b2bc4c6cd5"</getetag>
         *       </prop>
         *       <status xmlns="DAV:">HTTP/1.1 200 OK</status>
         *     </propstat>
         *   </response>
         * </multistatus>
         */

        const QString itemsMimeType = job->property("itemsMimeType").toString();
        QDomDocument document;
        document.setContent(davJob->responseData(), true);
        const QDomElement documentElement = document.documentElement();

        QDomElement responseElement = Utils::firstChildElementNS(documentElement, QStringLiteral("DAV:"), QStringLiteral("response"));
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

            const QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));

            // check whether it is a DAV collection ...
            const QDomElement resourcetypeElement = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("resourcetype"));
            if (!resourcetypeElement.isNull()) {
                const QDomElement collectionElement = Utils::firstChildElementNS(resourcetypeElement, QStringLiteral("DAV:"), QStringLiteral("collection"));
                if (!collectionElement.isNull()) {
                    responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
                    continue;
                }
            }

            // ... if not it is an item
            DavItem item;
            item.setContentType(itemsMimeType);

            // extract path
            const QDomElement hrefElement = Utils::firstChildElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("href"));
            const QString href = hrefElement.text();

            QUrl url = davJob->url();
            url.setUserInfo(QString());
            if (href.startsWith(QLatin1Char('/'))) {
                // href is only a path, use request url to complete
                url.setPath(href, QUrl::TolerantMode);
            } else {
                // href is a complete url
                url = QUrl::fromUserInput(href);
            }

            const QString itemUrl = url.toDisplayString();
            const auto [it, isInserted] = mSeenUrls.insert(itemUrl);
            if (!isInserted) {
                responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
                continue;
            }

            auto _url = url;
            _url.setUserInfo(mUrl.url().userInfo());
            item.setUrl(DavUrl(_url, mUrl.protocol()));

            // extract ETag
            const QDomElement getetagElement = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("getetag"));

            item.setEtag(getetagElement.text());

            mItems << item;

            if (mEtagCache->etagChanged(itemUrl, item.etag())) {
                mChangedItems << item;
            }

            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
        }
    }

    mDeletedItems.clear();

    const auto map = mEtagCache->etagCacheMap();
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        const QString remoteId = it.key();
        if (mSeenUrls.find(remoteId) == mSeenUrls.cend()) {
            mDeletedItems.append(remoteId);
        }
    }

    if (--mSubJobCount == 0) {
        emitResult();
    }
}
