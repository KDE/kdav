/*
   Copyright (c) 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
   Author: Kevin Ottens <kevin@kdab.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or ( at your option ) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QCoreApplication>

#include "common/davutils.h"
#include "common/davcollectionsfetchjob.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    DavUtils::DavUrl davUrl(QUrl(QStringLiteral("DAVURL")), DavUtils::CalDav);
    DavCollectionsFetchJob *job = new DavCollectionsFetchJob(davUrl);
    job->exec();

    foreach(const auto collection, job->collections()) {
        qDebug() << collection.url() << "PRIVS: " << collection.privileges();
    }

//    int retcode = app.quit();
//    return retcode;
}
