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

#include <QtCore/QUrl>
#include <QtXml/QDomDocument>

using namespace KDAV;

DavManager *DavManager::mSelf = Q_NULLPTR;

DavManager::DavManager()
{
}

DavManager::~DavManager()
{
    QMapIterator<Utils::Protocol, DavProtocolBase *> it(mProtocols);
    while (it.hasNext()) {
        it.next();
        delete it.value();
    }
}

DavManager *DavManager::self()
{
    if (!mSelf) {
        mSelf = new DavManager();
    }

    return mSelf;
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

const DavProtocolBase *DavManager::davProtocol(Utils::Protocol protocol)
{
    if (createProtocol(protocol)) {
        return mProtocols[ protocol ];
    } else {
        return Q_NULLPTR;
    }
}

bool DavManager::createProtocol(Utils::Protocol protocol)
{
    if (mProtocols.contains(protocol)) {
        return true;
    }

    switch (protocol) {
    case Utils::CalDav:
        mProtocols.insert(Utils::CalDav, new CaldavProtocol());
        break;
    case Utils::CardDav:
        mProtocols.insert(Utils::CardDav, new CarddavProtocol());
        break;
    case Utils::GroupDav:
        mProtocols.insert(Utils::GroupDav, new GroupdavProtocol());
        break;
    default:
        qCCritical(KDAV_LOG) << "Unknown protocol: " << static_cast<int>(protocol);
        return false;
    }

    return true;
}
