/*
  Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <KDAV/Utils>
#include <KDAV/DavCollectionDeleteJob>
#include <KDAV/DavCollectionModifyJob>
#include <KDAV/DavCollectionsFetchJob>
#include <KDAV/DavItemFetchJob>
#include <KDAV/DavItemCreateJob>
#include <KDAV/DavItemsFetchJob>
#include <KDAV/DavItemModifyJob>
#include <KDAV/DavItemDeleteJob>
#include <KDAV/DavItemsListJob>
#include <KDAV/EtagCache>
#include <KDAV/Utils>

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QCoreApplication>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QUrl mainUrl(QStringLiteral("https://apps.kolabnow.com/addressbooks/test1%40kolab.org"));
    mainUrl.setUserName(QStringLiteral("test1@kolab.org"));
    mainUrl.setPassword(QStringLiteral("Welcome2KolabSystems"));
    KDAV::DavUrl davUrl(mainUrl, KDAV::CardDav);

    auto *job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    for(const auto collection : job->collections()) {
        qDebug() << collection.displayName() << "PRIVS: " << collection.privileges();
        auto collectionUrl = collection.url();
        std::shared_ptr<KDAV::EtagCache> cache(new KDAV::EtagCache());
        int anz = -1;
        //Get all items in a collection add them to cache and make sure, that afterward no item is changed
        {
            auto itemListJob = new KDAV::DavItemsListJob(collectionUrl, cache);
            itemListJob->exec();
            anz = itemListJob->items().size();
            qDebug() << "items:" << itemListJob->items().size();
            qDebug() << "changed Items:" << itemListJob->changedItems().size();
            qDebug() << "deleted Items:" << itemListJob->deletedItems();
            for (const auto &item : itemListJob->changedItems()) {
                qDebug() << item.url().url() << item.contentType() << item.data();
                auto itemFetchJob = new KDAV::DavItemFetchJob(item);
                itemFetchJob->exec();
                const auto fetchedItem = itemFetchJob->item();
                qDebug() << fetchedItem.contentType() << fetchedItem.data();

                auto itemsFetchJob = new KDAV::DavItemsFetchJob(collectionUrl, QStringList() << item.url().toDisplayString());
                itemsFetchJob->exec();
                if (itemsFetchJob->item(item.url().toDisplayString()).contentType() != fetchedItem.contentType()) {       //itemsfetchjob do not get contentType
                    qDebug() << "Fetched same item but got different contentType:" << itemsFetchJob->item(item.url().toDisplayString()).contentType();
                }

                if (itemsFetchJob->item(item.url().toDisplayString()).data() != fetchedItem.data()) {
                    qDebug() << "Fetched same item but got different data:" << itemsFetchJob->item(item.url().toDisplayString()).data();
                }

                cache->setEtag(item.url().toDisplayString(), item.etag());
            }
            cache->setEtag(QStringLiteral("invalid"),QStringLiteral("invalid"));
        }
        {
            qDebug() << "second run: (should be empty).";
            auto itemListJob = new KDAV::DavItemsListJob(collectionUrl, cache);
            itemListJob->exec();
            if (itemListJob->items().size() != anz) {
                qDebug() << "Items have added/deleted on server.";
            }
            if (itemListJob->changedItems().size() != 0) {
                qDebug() << "Items have changed on server.";
            }
            if (itemListJob->deletedItems() != QStringList() << QStringLiteral("invalid")) {
                qDebug() << "more items deleted:" << itemListJob->deletedItems();
            }
        }
    }

    {
        QUrl url(QStringLiteral("https://apps.kolabnow.com/addressbooks/test1%40kolab.org/cbbf386d-7e9b-4e72-947d-0b813ea9b347/"));
        url.setUserInfo(mainUrl.userInfo());
        KDAV::DavUrl collectionUrl(url, KDAV::CardDav);
        auto collectionDeleteJob = new KDAV::DavCollectionDeleteJob(collectionUrl);
        collectionDeleteJob->exec();
        if (collectionDeleteJob->error()) {
            qDebug() << collectionDeleteJob->errorString();
        }
    }

    {
        QUrl url(QStringLiteral("https://apps.kolabnow.com/addressbooks/test1%40kolab.org/9290e784-c876-412f-8385-be292d64b2c6/"));
        url.setUserInfo(mainUrl.userInfo());
        KDAV::DavUrl testCollectionUrl(url, KDAV::CardDav);
        auto collectionModifyJob = new KDAV::DavCollectionModifyJob(testCollectionUrl);
        collectionModifyJob->setProperty(QStringLiteral("displayname"), QStringLiteral("test234"));
        collectionModifyJob->exec();
        if (collectionModifyJob->error()) {
            qDebug() << collectionModifyJob->errorString();
        }
    }

    //create element with "wrong put url" test if we get the correct url back
    {
        QUrl url(QStringLiteral("https://apps.kolabnow.com/addressbooks/test1%40kolab.org/9290e784-c876-412f-8385-be292d64b2c6/xxx.vcf"));
        url.setUserInfo(mainUrl.userInfo());
        KDAV::DavUrl testItemUrl(url, KDAV::CardDav);
        QByteArray data = "BEGIN:VCARD\r\nVERSION:3.0\r\nPRODID:-//Kolab//iRony DAV Server 0.3.1//Sabre//Sabre VObject 2.1.7//EN\r\nUID:12345678-1234-1234-1234-123456789abc\r\nFN:John Doe\r\nN:Doe;John;;;\r\nEMAIL;TYPE=INTERNET;TYPE=HOME:john.doe@example.com\r\nREV;VALUE=DATE-TIME:20161221T145611Z\r\nEND:VCARD\r\n";
        KDAV::DavItem item(testItemUrl, QStringLiteral("text/x-vcard"), data, QString());
        auto createJob = new KDAV::DavItemCreateJob(item);
        createJob->exec();
        if (createJob->error()) {
            qDebug() << createJob->errorString();
        }
        if (createJob->item().url().toDisplayString() != QStringLiteral("https://apps.kolabnow.com/addressbooks/test1%40kolab.org/9290e784-c876-412f-8385-be292d64b2c6/12345678-1234-1234-1234-123456789abc.vcf")) {
            qDebug() << "unexpected url" << createJob->item().url().url();
        }
    }

    {
        QUrl url(QStringLiteral("https://apps.kolabnow.com/addressbooks/test1%40kolab.org/9290e784-c876-412f-8385-be292d64b2c6/12345678-1234-1234-1234-123456789abc.vcf"));
        url.setUserInfo(mainUrl.userInfo());
        KDAV::DavUrl testItemUrl(url, KDAV::CardDav);
        QByteArray data = "BEGIN:VCARD\r\nVERSION:3.0\r\nPRODID:-//Kolab//iRony DAV Server 0.3.1//Sabre//Sabre VObject 2.1.7//EN\r\nUID:12345678-1234-1234-1234-123456789abc\r\nFN:John2 Doe\r\nN:Doe;John2;;;\r\nEMAIL;TYPE=INTERNET;TYPE=HOME:john2.doe@example.com\r\nREV;VALUE=DATE-TIME:20161221T145611Z\r\nEND:VCARD\r\n";
        KDAV::DavItem item(testItemUrl, QStringLiteral("text/x-vcard"), data, QString());
        auto modifyJob = new KDAV::DavItemModifyJob(item);
        modifyJob->exec();
        if (modifyJob->error()) {
            qDebug() << modifyJob->errorString();
        }
    }

    {
        QUrl url(QStringLiteral("https://apps.kolabnow.com/addressbooks/test1%40kolab.org/9290e784-c876-412f-8385-be292d64b2c6/12345678-1234-1234-1234-123456789abc.vcf"));
        url.setUserInfo(mainUrl.userInfo());
        KDAV::DavUrl testItemUrl(url, KDAV::CardDav);
        QByteArray data = "BEGIN:VCARD\r\nVERSION:3.0\r\nPRODID:-//Kolab//iRony DAV Server 0.3.1//Sabre//Sabre VObject 2.1.7//EN\r\nUID:12345678-1234-1234-1234-123456789abc\r\nFN:John2 Doe\r\nN:Doe;John2;;;\r\nEMAIL;TYPE=INTERNET;TYPE=HOME:john2.doe@example.com\r\nREV;VALUE=DATE-TIME:20161221T145611Z\r\nEND:VCARD\r\n";
        KDAV::DavItem item(testItemUrl, QStringLiteral("text/x-vcard"), data, QString());
        auto deleteJob = new KDAV::DavItemDeleteJob(item);
        deleteJob->exec();
        if (deleteJob->error()) {
            qDebug() << deleteJob->errorString();
        }
    }
}
