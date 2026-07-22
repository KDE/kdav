// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davpushregistrationjob.h"
#include "davjobbase_p.h"

#include "KDAV/DavUrl"
#include "daverror.h"
#include "davmultigetprotocol_p.h"
#include "davpushregistration.h"

#include <KIO/DavJob>
#include <KIO/StoredTransferJob>
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
    void onRegistrationDone(KJob *job);

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

void DavPushRegistrationJobPrivate::onRegistrationDone(KJob *doneJob)
{
    auto *job = qobject_cast<KIO::StoredTransferJob *>(doneJob);

    if (job->error() && job->error() != KIO::ERR_NO_CONTENT) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    const auto headers = job->queryMetaData("HTTP-Headers"_L1);
    for (const auto &header : headers.split("\n"_L1)) {
        auto sep = header.indexOf(": "_L1);
        Q_ASSERT(sep != -1);
        auto key = header.left(sep);
        auto value = header.mid(sep + 2);

        if (key.compare("location"_L1, Qt::CaseInsensitive) == 0) {
            mResponseRegistrationUrl = QUrl::fromUserInput(value);
        } else if (key.compare("expires"_L1, Qt::CaseInsensitive) == 0) {
            mResponseRegistrationExpiration = fromImfFixdate(value);
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

    KIO::StoredTransferJob *job = KIO::storedHttpPost(output.toUtf8(), d->mUrl.url(), KIO::HideProgressInfo);
    const QString header = "Content-Type: application/xml"_L1;
    job->addMetaData("customHTTPHeader"_L1, header);
    job->addMetaData("cookies"_L1, "none"_L1);
    job->addMetaData("no-auth-prompt"_L1, "true"_L1);
    job->addMetaData("PropagateHttpHeader"_L1, "true"_L1);

    connect(job, &KIO::StoredTransferJob::result, this, [d](KJob *job) {
        d->onRegistrationDone(job);
    });
    job->start();
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
