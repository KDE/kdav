/*
    Copyright (c) 2010 Grégory Oestreicher <greg@kamago.net>
    Based on DavItemsListJob which is copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

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

#include "davitemsfetchjob.h"
#include "davjobbase_p.h"

#include "davmanager_p.h"
#include "davmultigetprotocol_p.h"
#include "utils.h"
#include "daverror.h"

#include <KIO/DavJob>
#include <KIO/Job>

using namespace KDAV;

namespace KDAV {
class DavItemsFetchJobPrivate : public DavJobBasePrivate
{
public:
    DavUrl mCollectionUrl;
    QStringList mUrls;
    QMap<QString, DavItem> mItems;
};
}

DavItemsFetchJob::DavItemsFetchJob(const DavUrl &collectionUrl, const QStringList &urls, QObject *parent)
    : DavJobBase(new DavItemsFetchJobPrivate, parent)
{
    Q_D(DavItemsFetchJob);
    d->mCollectionUrl = collectionUrl;
    d->mUrls = urls;
}

void DavItemsFetchJob::start()
{
    Q_D(DavItemsFetchJob);
    const DavMultigetProtocol *protocol
        = dynamic_cast<const DavMultigetProtocol *>(DavManager::davProtocol(d->mCollectionUrl.protocol()));
    if (!protocol) {
        setError(ERR_NO_MULTIGET);
        setErrorTextFromDavError();
        emitResult();
        return;
    }

    const QDomDocument report = protocol->itemsReportQuery(d->mUrls)->buildQuery();
    KIO::DavJob *job = DavManager::self()->createReportJob(d->mCollectionUrl.url(), report, QStringLiteral("0"));
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    connect(job, &KIO::DavJob::result, this, &DavItemsFetchJob::davJobFinished);
}

DavItem::List DavItemsFetchJob::items() const
{
    Q_D(const DavItemsFetchJob);
    DavItem::List values;
    values.reserve(d->mItems.size());
    for (const auto &value : qAsConst(d->mItems)) {
        values << value;
    }
    return values;
}

DavItem DavItemsFetchJob::item(const QString &url) const
{
    Q_D(const DavItemsFetchJob);
    return d->mItems.value(url);
}

void DavItemsFetchJob::davJobFinished(KJob *job)
{
    Q_D(DavItemsFetchJob);
    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);
    const QString responseCodeStr = davJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty()
                             ? 0
                             : responseCodeStr.toInt();

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

    const DavMultigetProtocol *protocol
        = static_cast<const DavMultigetProtocol *>(DavManager::davProtocol(d->mCollectionUrl.protocol()));

    const QDomDocument document = davJob->response();
    const QDomElement documentElement = document.documentElement();

    QDomElement responseElement = Utils::firstChildElementNS(documentElement, QStringLiteral("DAV:"), QStringLiteral("response"));

    while (!responseElement.isNull()) {
        QDomElement propstatElement = Utils::firstChildElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("propstat"));

        if (propstatElement.isNull()) {
            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            continue;
        }

        // Check for errors
        const QDomElement statusElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("status"));
        if (!statusElement.text().contains(QLatin1String("200"))) {
            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            continue;
        }

        const QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));

        DavItem item;

        // extract path
        const QDomElement hrefElement = Utils::firstChildElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("href"));
        const QString href = hrefElement.text();

        QUrl url = davJob->url();
        if (href.startsWith(QLatin1Char('/'))) {
            // href is only a path, use request url to complete
            url.setPath(href, QUrl::TolerantMode);
        } else {
            // href is a complete url
            url = QUrl::fromUserInput(href);
        }

        auto _url = url;
        _url.setUserInfo(d->mCollectionUrl.url().userInfo());
        item.setUrl(DavUrl(_url, d->mCollectionUrl.protocol()));

        // extract etag
        const QDomElement getetagElement = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("getetag"));
        item.setEtag(getetagElement.text());

        // extract content
        const QDomElement dataElement = Utils::firstChildElementNS(propElement,
                                                                   protocol->responseNamespace(),
                                                                   protocol->dataTagName());

        if (dataElement.isNull()) {
            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            continue;
        }

        const QByteArray data = dataElement.firstChild().toText().data().toUtf8();
        if (data.isEmpty()) {
            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            continue;
        }

        item.setData(data);

        d->mItems.insert(item.url().toDisplayString(), item);
        responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
    }

    emitResult();
}
