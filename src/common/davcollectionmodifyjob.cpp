/*
    Copyright (c) 2010 Grégory Oestreicher <greg@kamago.net>

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

#include "davcollectionmodifyjob.h"
#include "davmanager.h"

#include "daverror.h"
#include "utils.h"
#include "davjob.h"

using namespace KDAV;

DavCollectionModifyJob::DavCollectionModifyJob(const DavUrl &url, QObject *parent)
    : DavJobBase(parent), mUrl(url)
{
}

void DavCollectionModifyJob::setProperty(const QString &prop, const QString &value, const QString &ns)
{
    QDomElement propElement;

    if (ns.isEmpty()) {
        propElement = mQuery.createElement(prop);
    } else {
        propElement = mQuery.createElementNS(ns, prop);
    }

    const QDomText textElement = mQuery.createTextNode(value);
    propElement.appendChild(textElement);

    mSetProperties << propElement;
}

void DavCollectionModifyJob::removeProperty(const QString &prop, const QString &ns)
{
    QDomElement propElement;

    if (ns.isEmpty()) {
        propElement = mQuery.createElement(prop);
    } else {
        propElement = mQuery.createElementNS(ns, prop);
    }

    mRemoveProperties << propElement;
}

void DavCollectionModifyJob::start()
{
    if (mSetProperties.isEmpty() && mRemoveProperties.isEmpty()) {
        setError(ERR_COLLECTIONMODIFY_NO_PROPERITES);
        setErrorTextFromDavError();
        emitResult();
        return;
    }

    QDomDocument mQuery;
    QDomElement propertyUpdateElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("propertyupdate"));
    mQuery.appendChild(propertyUpdateElement);

    if (!mSetProperties.isEmpty()) {
        QDomElement setElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("set"));
        propertyUpdateElement.appendChild(setElement);

        QDomElement propElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        setElement.appendChild(propElement);

        foreach (const QDomElement &element, mSetProperties) {
            propElement.appendChild(element);
        }
    }

    if (!mRemoveProperties.isEmpty()) {
        QDomElement removeElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("remove"));
        propertyUpdateElement.appendChild(removeElement);

        QDomElement propElement = mQuery.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        removeElement.appendChild(propElement);

        foreach (const QDomElement &element, mSetProperties) {
            propElement.appendChild(element);
        }
    }

    auto job = DavManager::self()->createPropPatchJob(mUrl.url(), mQuery);
    connect(job, &DavJob::result, this, &DavCollectionModifyJob::davJobFinished);
}

void DavCollectionModifyJob::davJobFinished(KJob *job)
{
    DavJob *davJob = qobject_cast<DavJob *>(job);
    const int responseCode = davJob->responseCode();

    if (davJob->error()) {
        setLatestResponseCode(responseCode);
        setError(ERR_COLLECTIONMODIFY);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();
        emitResult();
        return;
    }

    const QDomDocument response = davJob->response();
    QDomElement responseElement = Utils::firstChildElementNS(response.documentElement(), QStringLiteral("DAV:"), QStringLiteral("response"));

    bool hasError = false;
    QString errorText;

    // parse all propstats answers to get the eventual errors
    const QDomNodeList propstats = responseElement.elementsByTagNameNS(QStringLiteral("DAV:"), QStringLiteral("propstat"));
    for (int i = 0; i < propstats.length(); ++i) {
        const QDomElement propstatElement = propstats.item(i).toElement();
        const QDomElement statusElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("status"));

        const QString statusText = statusElement.text();
        if (statusText.contains(QStringLiteral("200"))) {
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
        const QDomElement responseDescriptionElement = Utils::firstChildElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("responsedescription"));
        if (!responseDescriptionElement.isNull()) {
            setJobErrorText(responseDescriptionElement.text());
        }
        setErrorTextFromDavError();
    }

    emitResult();
}
