// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davpushunregistrationjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"
#include "davurl.h"

#include <QNetworkReply>
#include <QUrl>

using namespace KDAV;
using namespace Qt::StringLiterals;

namespace KDAV
{

class DavPushUnregistrationJobPrivate : public DavJobBasePrivate
{
public:
    void onUnregistrationDone(QNetworkReply *reply);

    DavUrl mUrl;
    QUrl mRegistrationUrl;

    Q_DECLARE_PUBLIC(DavPushUnregistrationJob)
};
}

void DavPushUnregistrationJobPrivate::onUnregistrationDone(QNetworkReply *reply)
{
    reply->deleteLater();

    const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const bool isNotFound = reply->error() == QNetworkReply::ContentNotFoundError;

    if (!isNotFound && reply->error() != QNetworkReply::NoError) {
        setLatestResponseCode(responseCode);
        setError(ERR_DAVPUSH_UNREGISTER);
        setJobErrorText(replyErrorString(reply));
        setJobError(reply->error());
        setErrorTextFromDavError();
        emitResult();
        return;
    }

    emitResult();
}

DavPushUnregistrationJob::DavPushUnregistrationJob(const DavUrl &url, const QUrl &registrationUrl, QObject *parent)
    : DavJobBase(new DavPushUnregistrationJobPrivate, parent)
{
    Q_D(DavPushUnregistrationJob);
    d->mUrl = url;
    d->mRegistrationUrl = registrationUrl;
}

void DavPushUnregistrationJob::start()
{
    Q_D(DavPushUnregistrationJob);

    auto registrationUrl = d->mRegistrationUrl;
    registrationUrl.setUserInfo(d->mUrl.url().userInfo());
    QNetworkRequest request(registrationUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, DavManager::self()->userAgent());

    QNetworkReply *reply = DavManager::self()->networkAccessManager()->deleteResource(request);
    reply->setParent(this);
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->onUnregistrationDone(reply);
    });
}

#include "moc_davpushunregistrationjob.cpp"
