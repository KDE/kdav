/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davcollectionmodifyjob.h"
#include "davjobbase_p.h"
#include "davmanager_p.h"

#include "daverror.h"
#include "utils_p.h"

#include <KIO/DavJob>
#include <KIO/Job>

using namespace KDAV;

namespace KDAV
{
class DavCollectionModifyJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(KJob *job);

    DavUrl mUrl;
    QDomDocument mQuery;

    QVector<QDomElement> mSetProperties;
    QVector<QDomElement> mRemoveProperties;
};
}

DavCollectionModifyJob::DavCollectionModifyJob(const DavUrl &url, QObject *parent)
    : DavJobBase(new DavCollectionModifyJobPrivate, parent)
{
    Q_D(DavCollectionModifyJob);
    d->mUrl = url;
}

void DavCollectionModifyJob::setProperty(const QString &prop, const QString &value, const QString &ns)
{
    Q_D(DavCollectionModifyJob);
    QDomElement propElement;

    if (ns.isEmpty()) {
        propElement = d->mQuery.createElement(prop);
    } else {
        propElement = d->mQuery.createElementNS(ns, prop);
    }

    const QDomText textElement = d->mQuery.createTextNode(value);
    propElement.appendChild(textElement);

    d->mSetProperties << propElement;
}

void DavCollectionModifyJob::removeProperty(const QString &prop, const QString &ns)
{
    Q_D(DavCollectionModifyJob);
    QDomElement propElement;

    if (ns.isEmpty()) {
        propElement = d->mQuery.createElement(prop);
    } else {
        propElement = d->mQuery.createElementNS(ns, prop);
    }

    d->mRemoveProperties << propElement;
}

void DavCollectionModifyJob::start()
{
    Q_D(DavCollectionModifyJob);
    if (d->mSetProperties.isEmpty() && d->mRemoveProperties.isEmpty()) {
        setError(ERR_COLLECTIONMODIFY_NO_PROPERITES);
        d->setErrorTextFromDavError();
        emitResult();
        return;
    }

    QDomDocument mQuery;
    QDomElement propertyUpdateElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("propertyupdate"));
    mQuery.appendChild(propertyUpdateElement);

    if (!d->mSetProperties.isEmpty()) {
        QDomElement setElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("set"));
        propertyUpdateElement.appendChild(setElement);

        QDomElement propElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        setElement.appendChild(propElement);

        for (const QDomElement &element : std::as_const(d->mSetProperties)) {
            propElement.appendChild(element);
        }
    }

    if (!d->mRemoveProperties.isEmpty()) {
        QDomElement removeElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("remove"));
        propertyUpdateElement.appendChild(removeElement);

        QDomElement propElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        removeElement.appendChild(propElement);

        for (const QDomElement &element : std::as_const(d->mSetProperties)) {
            propElement.appendChild(element);
        }
    }

    KIO::DavJob *job = DavManager::self()->createPropPatchJob(d->mUrl.url(), mQuery.toString());
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    connect(job, &KIO::DavJob::result, this, [d](KJob *job) {
        d->davJobFinished(job);
    });
}

void DavCollectionModifyJobPrivate::davJobFinished(KJob *job)
{
    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);
    const QString responseCodeStr = davJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty() ? 0 : responseCodeStr.toInt();

    // KIO::DavJob does not set error() even if the HTTP status code is a 4xx or a 5xx
    if (davJob->error() || (responseCode >= 400 && responseCode < 600)) {
        setLatestResponseCode(responseCode);
        setError(ERR_COLLECTIONMODIFY);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();
        emitResult();
        return;
    }

    QDomDocument response;
    response.setContent(davJob->responseData(), true);
    QDomElement responseElement = Utils::firstChildElementNS(response.documentElement(), QStringLiteral("DAV:"), QStringLiteral("response"));

    bool hasError = false;

    // parse all propstats answers to get the eventual errors
    const QDomNodeList propstats = responseElement.elementsByTagNameNS(QStringLiteral("DAV:"), QStringLiteral("propstat"));
    for (int i = 0; i < propstats.length(); ++i) {
        const QDomElement propstatElement = propstats.item(i).toElement();
        const QDomElement statusElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("status"));

        const QString statusText = statusElement.text();
        if (statusText.contains(QLatin1String("200"))) {
            continue;
        } else {
            // Generic error
            hasError = true;
            break;
        }
    }

    if (hasError) {
        setError(ERR_COLLECTIONMODIFY_RESPONSE);

        // Trying to get more information about the error
        const QDomElement responseDescriptionElement =
            Utils::firstChildElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("responsedescription"));
        if (!responseDescriptionElement.isNull()) {
            setJobErrorText(responseDescriptionElement.text());
        }
        setErrorTextFromDavError();
    }

    emitResult();
}
