/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davcollectiondeletejob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"

#include <QNetworkReply>
#include <QNetworkRequest>

using namespace KDAV;

namespace KDAV
{
class DavCollectionDeleteJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(QNetworkReply *reply);

    DavUrl mUrl;
};
}

DavCollectionDeleteJob::DavCollectionDeleteJob(const DavUrl &url, QObject *parent)
    : DavJobBase(new DavCollectionDeleteJobPrivate, parent)
{
    Q_D(DavCollectionDeleteJob);
    d->mUrl = url;
}

void DavCollectionDeleteJob::start()
{
    Q_D(DavCollectionDeleteJob);
    QNetworkRequest request(d->mUrl.url());
    request.setHeader(QNetworkRequest::UserAgentHeader, DavManager::self()->userAgent());

    QNetworkReply *reply = DavManager::self()->networkAccessManager()->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->davJobFinished(reply);
    });
}

void DavCollectionDeleteJobPrivate::davJobFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        setLatestResponseCode(responseCode);
        setError(ERR_COLLECTIONDELETE);
        setJobErrorText(reply->errorString());
        setJobError(reply->error());
        setErrorTextFromDavError();
    }

    emitResult();
}

#include "moc_davcollectiondeletejob.cpp"
