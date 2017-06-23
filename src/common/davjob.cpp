/*
    Copyright (c) 2014 Gregory Oestreicher <greg@kamago.net>

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

#include "davjob.h"

#include "daverror.h"

using namespace KDAV;

class DavJobPrivate {
public:
    DavJobPrivate();

    QByteArray data;
    QDomDocument doc;
    QUrl url;

    QString location;
    QString etag;
    QString contentType;
    int responseCode = 0;
};

DavJobPrivate::DavJobPrivate()
{
}

DavJob::DavJob()
    : KJob()
    , d(std::unique_ptr<DavJobPrivate>(new DavJobPrivate()))
{
}

DavJob::DavJob(QNetworkReply *reply, QUrl url, QObject *parent)
    : KJob(parent),
    d(std::unique_ptr<DavJobPrivate>(new DavJobPrivate()))
{
    d->url = url;
    QObject::connect(reply, &QNetworkReply::readyRead, [=] () {
        d->data.append(reply->readAll());
    });
    QObject::connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] (QNetworkReply::NetworkError error) {
        qWarning() << "Error " << error << reply->errorString();
    });
    QObject::connect(reply, &QNetworkReply::redirected, [=] (const QUrl &url) {
        qWarning() << "Redirected: " << url;
    });
    QObject::connect(reply, &QNetworkReply::metaDataChanged, [=] () {
        // qWarning() << "Metadata changed: " << reply->rawHeaderPairs();
        d->location = reply->rawHeader("Location");
        d->etag = reply->rawHeader("ETag");
        //"text/x-vcard; charset=utf-8" -> "text/x-vcard"
        d->contentType = reply->rawHeader("Content-Type").split(';').first();
    });
    QObject::connect(reply, &QNetworkReply::finished, [=] () {
        d->doc.setContent(d->data, true);
        d->responseCode = reply->error();
        if (reply->error()) {
            setError(KJob::UserDefinedError);
            setErrorText(reply->errorString());
        }
        emitResult();
    });
}

DavJob::~DavJob()
{
}

void DavJob::start()
{
    // KIO::DavJob *job = DavManager::self()->createPropFindJob(url, collectionQuery);

    // KIO::DavJob *job = KIO::davPropFind(url, document, depth, KIO::HideProgressInfo | KIO::DefaultFlags);

    // // workaround needed, Depth: header doesn't seem to be correctly added
    // const QString header = QLatin1String("Content-Type: text/xml\r\nDepth: ") + depth;
    // job->addMetaData(QStringLiteral("customHTTPHeader"), header);
    // job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    // job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));
    // job->setProperty("extraDavDepth", QVariant::fromValue(depth));

    // connect(job, &KIO::DavJob::result, this, &DavCollectionsFetchJob::collectionsFetchFinished);
    // job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
}

QDomDocument DavJob::response()
{
    return d->doc;
}

QByteArray DavJob::data()
{
    return d->data;
}

QUrl DavJob::url()
{
    return d->url;
}

QString DavJob::getLocationHeader()
{
    return d->location;
}

QString DavJob::getETagHeader()
{
    return d->etag;
}

QString DavJob::getContentTypeHeader()
{
    return d->contentType;
}

int DavJob::responseCode()
{
    return d->responseCode;
}
