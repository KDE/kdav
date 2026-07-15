// SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davcollectioncreatejob.h"
#include "davjobbase_p.h"
#include "davmultigetprotocol_p.h"

#include "daverror.h"
#include "davmanager_p.h"
#include "davpushdontnotify.h"

#include <KIO/DavJob>
#include <KJob>
#include <QColor>
#include <QXmlStreamWriter>

using namespace KDAV;
using namespace Qt::StringLiterals;

namespace KDAV
{
class DavCollectionCreateJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(KJob *job);

    DavCollection mCollection;
    int mRedirectCount = 0;
    DavPushDontNotify mPushDontNotify;

    Q_DECLARE_PUBLIC(DavCollectionCreateJob)
};
}

DavCollectionCreateJob::DavCollectionCreateJob(const DavCollection &collection, QObject *parent)
    : DavJobBase(new DavCollectionCreateJobPrivate, parent)
{
    Q_D(DavCollectionCreateJob);
    d->mCollection = collection;
}

void DavCollectionCreateJob::setPushDontNotify(const DavPushDontNotify &dontNotify)
{
    Q_D(DavCollectionCreateJob);
    d->mPushDontNotify = dontNotify;
}

DavPushDontNotify DavCollectionCreateJob::pushDontNotify() const
{
    Q_D(const DavCollectionCreateJob);
    return d->mPushDontNotify;
}

void DavCollectionCreateJob::start()
{
    Q_D(DavCollectionCreateJob);

    const DavMultigetProtocol *protocol = dynamic_cast<const DavMultigetProtocol *>(DavManager::davProtocol(d->mCollection.url().protocol()));
    if (!protocol) {
        setError(ERR_NO_MULTIGET);
        d->setErrorTextFromDavError();
        emitResult();
        return;
    }

    QString output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    protocol->writeMkCol(writer, d->mCollection);

    const auto headers = d->mPushDontNotify.isNull() ? QStringList{} : QStringList{d->mPushDontNotify.davHeader()};
    auto job = DavManager::self()->createMkColJob(collectionUrl(), output, headers);

    connect(job, &KJob::result, this, [d](KJob *job) {
        d->davJobFinished(job);
    });
}

DavCollection DavCollectionCreateJob::collection() const
{
    Q_D(const DavCollectionCreateJob);
    return d->mCollection;
}

QUrl DavCollectionCreateJob::collectionUrl() const
{
    Q_D(const DavCollectionCreateJob);
    return d->mCollection.url().url();
}

void DavCollectionCreateJobPrivate::davJobFinished(KJob *job)
{
    Q_Q(DavCollectionCreateJob);
    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);

    const QString responseCodeStr = davJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty() ? 0 : responseCodeStr.toInt();

    if (davJob->error() || responseCode >= 400) {
        setLatestResponseCode(responseCode);
        setError(ERR_COLLECTIONCREATE);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();
        emitResult();
        return;
    }

    // The 'Location:' HTTP header is used to indicate the new URL
    const QStringList allHeaders = davJob->queryMetaData(QStringLiteral("HTTP-Headers")).split(QLatin1Char('\n'));
    QString location;
    for (const QString &header : allHeaders) {
        if (header.startsWith(QLatin1String("location:"), Qt::CaseInsensitive)) {
            location = header.section(QLatin1Char(' '), 1);
        }
    }

    QUrl url;
    if (location.isEmpty()) {
        url = davJob->url();
    } else if (location.startsWith(QLatin1Char('/'))) {
        url = davJob->url();
        url.setPath(location, QUrl::TolerantMode);
    } else {
        url = QUrl::fromUserInput(location);
    }

    if (responseCode == 301 || responseCode == 302 || responseCode == 307 || responseCode == 308) {
        if (mRedirectCount > 4) {
            setLatestResponseCode(responseCode);
            setError(DavCollectionCreateJob::UserDefinedError + responseCode);
            emitResult();
        } else {
            QUrl _collectionUrl(url);
            _collectionUrl.setUserInfo(q->collectionUrl().userInfo());
            mCollection.setUrl(DavUrl(_collectionUrl, mCollection.url().protocol()));

            ++mRedirectCount;
            q->start();
        }

        return;
    }

    url.setUserInfo(q->collectionUrl().userInfo());
    mCollection.setUrl(DavUrl(url, mCollection.url().protocol()));
    emitResult();
}

#include "moc_davcollectioncreatejob.cpp"
