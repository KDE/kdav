/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davmanager_p.h"

#include "protocols/caldavprotocol_p.h"
#include "protocols/carddavprotocol_p.h"
#include "protocols/groupdavprotocol_p.h"

#include <KIO/DavJob>

#include "libkdav_debug.h"

#include <QUrl>

using namespace KDAV;

DavManager::DavManager() = default;
DavManager::~DavManager() = default;

DavManager *DavManager::self()
{
    static DavManager sSelf;
    return &sSelf;
}

KIO::DavJob *DavManager::createPropFindJob(const QUrl &url, const QString &document, const QString &depth) const
{
    KIO::DavJob *job = KIO::davPropFind(url, document, depth, KIO::HideProgressInfo | KIO::DefaultFlags);

    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));
    job->setProperty("davDepth", QVariant::fromValue(depth));

    return job;
}

KIO::DavJob *DavManager::createReportJob(const QUrl &url, const QString &document, const QString &depth) const
{
    KIO::DavJob *job = KIO::davReport(url, document, depth, KIO::HideProgressInfo | KIO::DefaultFlags);

    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));
    job->setProperty("davDepth", QVariant::fromValue(depth));

    return job;
}

KIO::DavJob *DavManager::createPropPatchJob(const QUrl &url, const QString &document) const
{
    KIO::DavJob *job = KIO::davPropPatch(url, document, KIO::HideProgressInfo | KIO::DefaultFlags);
    const QString header = QStringLiteral("Content-Type: text/xml");
    job->addMetaData(QStringLiteral("customHTTPHeader"), header);
    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));
    return job;
}

const DavProtocolBase *DavManager::davProtocol(Protocol protocol)
{
    const auto d = DavManager::self();
    if (!d->mProtocols[protocol]) {
        switch (protocol) {
        case KDAV::CalDav:
            d->mProtocols[KDAV::CalDav].reset(new CaldavProtocol());
            break;
        case KDAV::CardDav:
            d->mProtocols[KDAV::CardDav].reset(new CarddavProtocol());
            break;
        case KDAV::GroupDav:
            d->mProtocols[KDAV::GroupDav].reset(new GroupdavProtocol());
            break;
        default:
            qCCritical(KDAV_LOG) << "Unknown protocol: " << static_cast<int>(protocol);
            return nullptr;
        }
    }

    return d->mProtocols[protocol].get();
}
