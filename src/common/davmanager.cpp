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
#include "davjob.h"
#include "qwebdavlib/qwebdav.h"

#include "libkdav_debug.h"

#include <QtCore/QUrl>
#include <QtXml/QDomDocument>

using namespace KDAV;

DavManager *DavManager::mSelf = nullptr;

DavManager::DavManager()
{
    mWebDav = new QWebdav;
    QObject::connect(mWebDav, &QWebdav::errorChanged, [=] (const QString &error) {
        qWarning() << "Got error " << error;
    });
}

DavManager::~DavManager()
{
    QMapIterator<Protocol, DavProtocolBase *> it(mProtocols);
    while (it.hasNext()) {
        it.next();
        delete it.value();
    }
    delete mWebDav;
}

DavManager *DavManager::self()
{
    if (!mSelf) {
        mSelf = new DavManager();
    }

    return mSelf;
}

void DavManager::setConnectionSettings(const QUrl &url)
{
    mWebDav->setConnectionSettings(url.scheme() == "https" ? QWebdav::HTTPS : QWebdav::HTTP, url.host(), "/", url.userName(), url.password(), url.port(0));
}

DavJob *DavManager::createPropFindJob(const QUrl &url, const QDomDocument &document, const QString &depth)
{
    setConnectionSettings(url);
    auto reply = mWebDav->propfind(url.path(), document.toByteArray(), depth.toInt());
    return new DavJob{reply, url};
}

DavJob *DavManager::createReportJob(const QUrl &url, const QDomDocument &document, const QString &depth)
{
    setConnectionSettings(url);
    auto reply = mWebDav->report(url.path(), document.toByteArray(), depth.toInt());
    return new DavJob{reply, url};
}

DavJob *DavManager::createDeleteJob(const QUrl &url)
{
    setConnectionSettings(url);
    auto reply = mWebDav->remove(url.path());
    return new DavJob{reply, url};
}


DavJob *DavManager::createGetJob(const QUrl &url)
{
    setConnectionSettings(url);
    // Work around a strange bug in Zimbra (seen at least on CE 5.0.18) : if the user-agent
    // contains "Mozilla", some strange debug data is displayed in the shared calendars.
    // This kinda mess up the events parsing...
    auto reply = mWebDav->get(url.path(), {{"User-Agent", "KDE DAV groupware client"}});
    return new DavJob{reply, url};
}

DavJob *DavManager::createPropPatchJob(const QUrl &url, const QDomDocument &document)
{
    setConnectionSettings(url);
    auto reply = mWebDav->proppatch(url.path(), document.toByteArray());
    return new DavJob{reply, url};
}

DavJob *DavManager::createCreateJob(const QByteArray &data, const QUrl &url, const QByteArray &contentType)
{
    setConnectionSettings(url);
    auto reply = mWebDav->put(url.path(), data, {{"Content-Type", contentType}, {"If-None-Match", "*"}});
    return new DavJob{reply, url};
}

DavJob *DavManager::createModifyJob(const QByteArray &data, const QUrl &url, const QByteArray &contentType, const QByteArray &etag)
{
    setConnectionSettings(url);
    auto reply = mWebDav->put(url.path(), data, {{"Content-Type", contentType}, {"If-Match", etag}});
    return new DavJob{reply, url};
}

const DavProtocolBase *DavManager::davProtocol(Protocol protocol)
{
    if (createProtocol(protocol)) {
        return mProtocols[ protocol ];
    } else {
        return nullptr;
    }
}

bool DavManager::createProtocol(Protocol protocol)
{
    if (mProtocols.contains(protocol)) {
        return true;
    }

    switch (protocol) {
    case KDAV::CalDav:
        mProtocols.insert(KDAV::CalDav, new CaldavProtocol());
        break;
    case KDAV::CardDav:
        mProtocols.insert(KDAV::CardDav, new CarddavProtocol());
        break;
    case KDAV::GroupDav:
        mProtocols.insert(KDAV::GroupDav, new GroupdavProtocol());
        break;
    default:
        qCCritical(KDAV_LOG) << "Unknown protocol: " << static_cast<int>(protocol);
        return false;
    }

    return true;
}
