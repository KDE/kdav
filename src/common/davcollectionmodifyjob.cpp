/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davcollectionmodifyjob.h"
#include "davjobbase_p.h"
#include "davmanager_p.h"

#include "daverror.h"
#include "utils_p.h"

#include <QNetworkReply>
#include <QNetworkRequest>

using namespace KDAV;

namespace KDAV
{
class DavCollectionModifyJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(QNetworkReply *reply);

    DavUrl mUrl;
    QDomDocument mQuery;

    QList<QDomElement> mSetProperties;
    QList<QDomElement> mRemoveProperties;
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

    QNetworkReply *reply = DavManager::self()->createPropPatchJob(d->mUrl.url(), mQuery.toString());
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->davJobFinished(reply);
    });
}

void DavCollectionModifyJobPrivate::davJobFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError || (responseCode >= 400 && responseCode < 600)) {
        setLatestResponseCode(responseCode);
        setError(ERR_COLLECTIONMODIFY);
        setJobErrorText(reply->errorString());
        setJobError(reply->error());
        setErrorTextFromDavError();
        emitResult();
        return;
    }

    QDomDocument response;
    response.setContent(reply->readAll(), QDomDocument::ParseOption::UseNamespaceProcessing);
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

#include "moc_davcollectionmodifyjob.cpp"
