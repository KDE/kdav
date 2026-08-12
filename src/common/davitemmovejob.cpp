/*
    SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemmovejob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davitemfetchjob.h"
#include "davmanager_p.h"

#include <QNetworkReply>
#include <QNetworkRequest>

using namespace KDAV;

namespace KDAV
{
class DavItemMoveJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(QNetworkReply *reply);

    DavItem mItem;
    QUrl mDestination;

    DavItem mFreshItem;
    int mFreshResponseCode = -1;
};
}

DavItemMoveJob::DavItemMoveJob(const DavItem &item, const QUrl &destination, QObject *parent)
    : DavJobBase(new DavItemMoveJobPrivate, parent)
{
    Q_D(DavItemMoveJob);
    d->mItem = item;
    d->mDestination = destination;
}

void DavItemMoveJob::start()
{
    Q_D(DavItemMoveJob);
    auto request = QNetworkRequest(d->mItem.url().url());
    request.setHeader(QNetworkRequest::UserAgentHeader, DavManager::self()->userAgent());
    request.setRawHeader("Destination", d->mDestination.url().toUtf8());
    request.setRawHeader("Overwrite", "T");

    auto reply = DavManager::self()->networkAccessManager()->sendCustomRequest(request, "MOVE");
    reply->setParent(this);
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->davJobFinished(reply);
    });
}

void DavItemMoveJobPrivate::davJobFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        setLatestResponseCode(responseCode);
        setError(ERR_ITEMMOVE);
        setJobErrorText(reply->errorString());
        setJobError(reply->error());
        setErrorTextFromDavError();
    }

    emitResult();
}

#include "moc_davitemmovejob.cpp"
