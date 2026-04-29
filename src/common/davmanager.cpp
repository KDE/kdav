/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davmanager_p.h"

#include "protocols/caldavprotocol_p.h"
#include "protocols/carddavprotocol_p.h"
#include "protocols/groupdavprotocol_p.h"

#include "libkdav_debug.h"

#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>

using namespace KDAV;
using namespace Qt::StringLiterals;

DavManager::DavManager()
    : mNam(std::make_unique<QNetworkAccessManager>())
{
    mNam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    mNam->enableStrictTransportSecurityStore(true, QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + u"/hsts/"_s);
    mNam->setStrictTransportSecurityEnabled(true);

    QObject::connect(mNam.get(), &QNetworkAccessManager::authenticationRequired, [](QNetworkReply *reply, QAuthenticator *auth) {
        const QUrl url = reply->request().url();
        if (!url.userName().isEmpty()) {
            auth->setUser(url.userName());
            auth->setPassword(url.password());
        }
    });
}

DavManager::~DavManager() = default;

DavManager *DavManager::self()
{
    static DavManager sSelf;
    return &sSelf;
}

QNetworkReply *DavManager::createPropFindJob(const QUrl &url, const QString &document, const QString &depth) const
{
    return sendDavRequest("PROPFIND", url, document, depth);
}

QNetworkReply *DavManager::createReportJob(const QUrl &url, const QString &document, const QString &depth) const
{
    return sendDavRequest("REPORT", url, document, depth);
}

QNetworkReply *DavManager::createPropPatchJob(const QUrl &url, const QString &document) const
{
    return sendDavRequest("PROPPATCH", url, document);
}

QNetworkAccessManager *DavManager::networkAccessManager() const
{
    return mNam.get();
}

QNetworkReply *DavManager::sendDavRequest(const QByteArray &method, const QUrl &url, const QString &document, const QString &depth) const
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, u"text/xml; charset=utf-8"_s);
    if (!depth.isEmpty()) {
        request.setRawHeader("Depth", depth.toUtf8());
    }
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent());
    return mNam->sendCustomRequest(request, method, QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n") + document.toUtf8());
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

QString DavManager::userAgent() const
{
    // Work around a strange bug in Zimbra (seen at least on CE 5.0.18) : if the user-agent
    // contains "Mozilla", some strange debug data is displayed in the shared calendars.
    // This kinda mess up the events parsing...
    return u"KDE DAV groupware client"_s;
}
