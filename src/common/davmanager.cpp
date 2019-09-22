/*
    Copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

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

#include "davmanager.h"

#include "protocols/caldavprotocol.h"
#include "protocols/carddavprotocol.h"
#include "protocols/groupdavprotocol.h"

#include <KIO/DavJob>

#include "libkdav_debug.h"

#include <QUrl>
#include <QDomDocument>

using namespace KDAV;

namespace KDAV {
class DavManagerPrivate
{
public:
    std::unique_ptr<DavProtocolBase> mProtocols[PROTOCOL_COUNT];
};
}

DavManager::DavManager() :
    d(new DavManagerPrivate)
{
}

DavManager::~DavManager() = default;

DavManager *DavManager::self()
{
    static DavManager sSelf;
    return &sSelf;
}

KIO::DavJob *DavManager::createPropFindJob(const QUrl &url, const QDomDocument &document, const QString &depth) const
{
    KIO::DavJob *job = KIO::davPropFind(url, document, depth, KIO::HideProgressInfo | KIO::DefaultFlags);

    // workaround needed, Depth: header doesn't seem to be correctly added
    const QString header = QLatin1String("Content-Type: text/xml\r\nDepth: ") + depth;
    job->addMetaData(QStringLiteral("customHTTPHeader"), header);
    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));
    job->setProperty("extraDavDepth", QVariant::fromValue(depth));

    return job;
}

KIO::DavJob *DavManager::createReportJob(const QUrl &url, const QDomDocument &document, const QString &depth) const
{
    KIO::DavJob *job = KIO::davReport(url, document.toString(), depth, KIO::HideProgressInfo | KIO::DefaultFlags);

    // workaround needed, Depth: header doesn't seem to be correctly added
    const QString header = QLatin1String("Content-Type: text/xml\r\nDepth: ") + depth;
    job->addMetaData(QStringLiteral("customHTTPHeader"), header);
    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));
    job->setProperty("extraDavDepth", QVariant::fromValue(depth));

    return job;
}

KIO::DavJob *DavManager::createPropPatchJob(const QUrl &url, const QDomDocument &document) const
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
