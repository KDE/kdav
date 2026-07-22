// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davpushregistrationjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"
#include "davmultigetprotocol_p.h"
#include "davpushregistration.h"
#include "davurl.h"

#include <QNetworkReply>
#include <QUrl>
#include <QXmlStreamWriter>
#include <qhttpheaders.h>

using namespace KDAV;
using namespace Qt::StringLiterals;

namespace KDAV
{

class DavPushRegistrationJobPrivate : public DavJobBasePrivate
{
public:
    void writeDavPushRegister(QXmlStreamWriter &writer) const;
    void onRegistrationDone(QNetworkReply *reply);

    DavUrl mUrl;
    DavPushRegistration mPushRegistration;

    QUrl mResponseRegistrationUrl;
    QDateTime mResponseRegistrationExpiration;

    Q_DECLARE_PUBLIC(DavPushRegistrationJob)
};
}

static QString imfFixdate(const QDateTime &dt)
{
    QHttpHeaders headers;
    headers.setDateTimeValue("datetime"_L1, dt);
    return QString::fromUtf8(headers.value("datetime"_L1));
}

static QDateTime fromImfFixdate(const QString &str)
{
    QHttpHeaders headers;
    headers.append("datetime"_L1, str.toUtf8());
    auto res = headers.dateTimeValue("datetime"_L1);
    return res ? *res : QDateTime();
}

void DavPushRegistrationJobPrivate::writeDavPushRegister(QXmlStreamWriter &writer) const
{
    writer.writeStartDocument();
    writer.writeStartElement("push-register"_L1);
    writer.writeDefaultNamespace(Xml::webdavpushNS);
    writer.writeNamespace(Xml::davNS, "D"_L1);
    writer.writeNamespace(Xml::icalNS, "I"_L1);

    writer.writeStartElement("subscription"_L1);
    writer.writeStartElement("web-push-subscription"_L1);
    writer.writeTextElement("push-resource"_L1, mPushRegistration.pushEndpoint().toDisplayString().toUtf8());
    writer.writeTextElement("content-encoding"_L1, "aes128gcm"_L1);
    writer.writeStartElement("subscription-public-key"_L1);
    writer.writeAttribute("type"_L1, "p256dh"_L1);
    writer.writeCharacters(mPushRegistration.subscriptionPublicKey());
    writer.writeEndElement();
    writer.writeTextElement("auth-secret"_L1, mPushRegistration.authToken());
    writer.writeEndElement(); // web-push-subscription
    writer.writeEndElement(); // subscription

    writer.writeStartElement("trigger"_L1);
    writer.writeStartElement("content-update"_L1);
    writer.writeTextElement("D:depth"_L1, "infinity"_L1);
    writer.writeEndElement(); // content-update
    writer.writeStartElement("property-update"_L1);
    writer.writeTextElement("D:depth"_L1, "0"_L1);
    writer.writeStartElement("D:prop"_L1);
    writer.writeEmptyElement("D:displayname"_L1);
    writer.writeEmptyElement("I:calendar-color"_L1);
    writer.writeEndElement(); // prop
    writer.writeEndElement(); // property-update
    writer.writeEndElement(); // trigger

    writer.writeTextElement("expires"_L1, imfFixdate(mPushRegistration.expiration()));
    writer.writeEndElement(); // push-register
    writer.writeEndDocument();
}

void DavPushRegistrationJobPrivate::onRegistrationDone(QNetworkReply *reply)
{
    reply->deleteLater();

    const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->error() != QNetworkReply::NoError) {
        setLatestResponseCode(responseCode);
        setError(ERR_DAVPUSH_REGISTER);
        setJobErrorText(reply->errorString());
        setJobError(reply->error());
        setErrorTextFromDavError();
        emitResult();
        return;
    }

    for (const auto &[headerName, headerValue] : reply->headers().toListOfPairs()) {
        if (headerName.compare("location"_L1, Qt::CaseInsensitive) == 0) {
            mResponseRegistrationUrl = QUrl::fromUserInput(QString::fromUtf8(headerValue));
        } else if (headerName.compare("expires"_L1, Qt::CaseInsensitive) == 0) {
            mResponseRegistrationExpiration = fromImfFixdate(QString::fromUtf8(headerValue));
        }
    }

    emitResult();
}

DavPushRegistrationJob::DavPushRegistrationJob(const DavUrl &url, const DavPushRegistration &davPushRegistration, QObject *parent)
    : DavJobBase(new DavPushRegistrationJobPrivate, parent)
{
    Q_D(DavPushRegistrationJob);
    d->mUrl = url;
    d->mPushRegistration = davPushRegistration;
}

void DavPushRegistrationJob::start()
{
    Q_D(DavPushRegistrationJob);

    QString output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    d->writeDavPushRegister(writer);

    QNetworkRequest request(d->mUrl.url());
    request.setHeader(QNetworkRequest::UserAgentHeader, DavManager::self()->userAgent());
    request.setHeader(QNetworkRequest::ContentTypeHeader, u"text/xml; charset=utf-8"_s);

    QNetworkReply *reply = DavManager::self()->networkAccessManager()->post(request, output.toUtf8());
    reply->setParent(this);
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->onRegistrationDone(reply);
    });
}

QUrl DavPushRegistrationJob::registrationUrl() const
{
    Q_D(const DavPushRegistrationJob);
    return d->mResponseRegistrationUrl;
}

QDateTime DavPushRegistrationJob::expirationDate() const
{
    Q_D(const DavPushRegistrationJob);
    return d->mResponseRegistrationExpiration;
}

#include "moc_davpushregistrationjob.cpp"
