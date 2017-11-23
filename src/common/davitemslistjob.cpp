/*
    Copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

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

#include "davitemslistjob.h"

#include "daverror.h"
#include "davmanager.h"
#include "davprotocolbase.h"
#include "davurl.h"
#include "utils.h"
#include "etagcache.h"

#include <KIO/DavJob>
#include <KIO/Job>


using namespace KDAV;

class DavItemsListJobPrivate {
public:
    DavItemsListJobPrivate(const DavUrl &url, const std::shared_ptr<EtagCache> &cache);

    DavUrl mUrl;
    std::shared_ptr<EtagCache> mEtagCache;
    QStringList mMimeTypes;
    QString mRangeStart;
    QString mRangeEnd;
    DavItem::List mItems;
    QSet<QString> mSeenUrls; // to prevent events duplication with some servers
    DavItem::List mChangedItems;
    QStringList mDeletedItems;
    uint mSubJobCount;
};

DavItemsListJobPrivate::DavItemsListJobPrivate(const DavUrl &url, const std::shared_ptr<EtagCache> &cache)
    : mUrl(url)
    , mEtagCache(cache)
    , mSubJobCount(0)
{
}


DavItemsListJob::DavItemsListJob(const DavUrl &url, const std::shared_ptr<EtagCache> &cache, QObject *parent)
    : DavJobBase(parent)
    , d(std::unique_ptr<DavItemsListJobPrivate>(new DavItemsListJobPrivate(url, cache)))
{
}

DavItemsListJob::~DavItemsListJob()
{
}

void DavItemsListJob::setContentMimeTypes(const QStringList &types)
{
    d->mMimeTypes = types;
}

void DavItemsListJob::setTimeRange(const QString &start, const QString &end)
{
    d->mRangeStart = start;
    d->mRangeEnd = end;
}

void DavItemsListJob::start()
{
    const DavProtocolBase *protocol = DavManager::self()->davProtocol(d->mUrl.protocol());
    Q_ASSERT(protocol);
    QVectorIterator<XMLQueryBuilder::Ptr> it(protocol->itemsQueries());

    while (it.hasNext()) {
        XMLQueryBuilder::Ptr builder = it.next();
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
                KIO::DavJob *job = DavManager::self()->createReportJob(d->mUrl.url(), props);
                job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
                job->setProperty("davType", QStringLiteral("report"));
                job->setProperty("itemsMimeType", mimeType);
                connect(job, &KIO::DavJob::result, this, &DavItemsListJob::davJobFinished);
            } else {
                KIO::DavJob *job = DavManager::self()->createPropFindJob(d->mUrl.url(), props);
                job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
                job->setProperty("davType", QStringLiteral("propFind"));
                job->setProperty("itemsMimeType", mimeType);
                connect(job, &KIO::DavJob::result, this, &DavItemsListJob::davJobFinished);
            }
        }
    }

    if (d->mSubJobCount == 0) {
        setError(ERR_ITEMLIST_NOMIMETYPE);
        setErrorTextFromDavError();
        emitResult();
    }
}

DavItem::List DavItemsListJob::items() const
{
    return d->mItems;
}

DavItem::List DavItemsListJob::changedItems() const
{
    return d->mChangedItems;
}

QStringList DavItemsListJob::deletedItems() const
{
    return d->mDeletedItems;
}

void DavItemsListJob::davJobFinished(KJob *job)
{
    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);
    const int responseCode = davJob->queryMetaData(QStringLiteral("responsecode")).isEmpty() ?
                             0 :
                             davJob->queryMetaData(QStringLiteral("responsecode")).toInt();

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
        const QDomDocument document = davJob->response();
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
                    if (statusElement.text().contains(QStringLiteral("200"))) {
                        propstatElement = propstatCandidate;
                    }
                }
            }

            if (propstatElement.isNull()) {
                responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
                continue;
            }

            const QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));

            // check whether it is a dav collection ...
            const QDomElement resourcetypeElement = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("resourcetype"));
            if (!responseElement.isNull()) {
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

            QUrl url = QUrl::fromUserInput(href);
            url.setUserInfo(QString());
            if (href.startsWith(QLatin1Char('/'))) {
                // href is a relative URL (i.e. missing scheme, hostname and so on)
                // QUrl would treat this as a file URL be default, avoid that by setting
                // empty scheme.
                // Note: We don't want to expand this into a full URL (including hostname)
                // as some CalDav implementations don't seem to handle that correctly.
                url.setScheme(QString());
            }

            QString itemUrl = url.toDisplayString();
            if (d->mSeenUrls.contains(itemUrl)) {
                responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
                continue;
            }

            d->mSeenUrls << itemUrl;
            auto _url = url;
            _url.setUserInfo(d->mUrl.url().userInfo());
            item.setUrl(DavUrl(_url, d->mUrl.protocol()));

            // extract etag
            const QDomElement getetagElement = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("getetag"));

            item.setEtag(getetagElement.text());

            d->mItems << item;

            if (d->mEtagCache->etagChanged(itemUrl, item.etag())) {
                d->mChangedItems << item;
            }

            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
        }
    }

    QSet<QString> removed = d->mEtagCache->urls().toSet();
    removed.subtract(d->mSeenUrls);
    d->mDeletedItems = removed.toList();

    if (--d->mSubJobCount == 0) {
        emitResult();
    }
}

