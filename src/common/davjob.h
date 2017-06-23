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

#ifndef KDAV_DAVJOB_H
#define KDAV_DAVJOB_H

#include <memory>

#include "kpimkdav_export.h"

#include <KCoreAddons/KJob>
#include <QDomDocument>
#include <QUrl>
#include <QNetworkReply>

class DavJobPrivate;

namespace KIO {
    class DavJob;
}

namespace KDAV
{
class Error;

    /* API
     * queryMetaData(QString) -> QString
     * KJob::error
     * url
     * QDomDocument &response();
     */
class KPIMKDAV_EXPORT DavJob : public KJob
{
    Q_OBJECT

public:
    DavJob();
    explicit DavJob(KIO::DavJob *job, QObject *parent = nullptr);
    explicit DavJob(QNetworkReply *reply, QUrl url, QObject *parent = nullptr);
    ~DavJob();

    virtual void start() Q_DECL_OVERRIDE;

    QDomDocument response();
    QByteArray data();
    QUrl url();

    int responseCode();

    QString getLocationHeader();
    QString getETagHeader();
    QString getContentTypeHeader();

private:
    std::unique_ptr<DavJobPrivate> d;
};

}

#endif
