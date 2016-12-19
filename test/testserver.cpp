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
#include <KDAV/DavCollectionsFetchJob>
#include <KDAV/DavItemFetchJob>
#include <KDAV/DavItemsFetchJob>
#include <KDAV/DavItemsListJob>
#include <KDAV/EtagCache>

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QCoreApplication>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QUrl mainUrl(QStringLiteral("http://kolab/iRony/addressbooks/john.doe%40example.org"));
    mainUrl.setUserName(QStringLiteral("john.doe@example.org"));
    mainUrl.setPassword(QStringLiteral("Welcome2KolabSystems"));
    KDAV::DavUrl davUrl(mainUrl, KDAV::CardDav);

    auto *job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    foreach(const auto collection, job->collections()) {
        qDebug() << collection.url().toDisplayString() << "PRIVS: " << collection.privileges();
        auto collectionUrl = collection.url();
        KDAV::EtagCache cache;
        int anz = -1;
        //Get all items in a collection add them to cache and make sure, that afterward no item is changed
        {
            auto itemListJob = new KDAV::DavItemsListJob(collectionUrl, &cache);
            itemListJob->exec();
            anz = itemListJob->items().size();
            qDebug() << "items:" << itemListJob->items().size();
            qDebug() << "changed Items:" << itemListJob->changedItems().size();
            qDebug() << "deleted Items:" << itemListJob->deletedItems();
            foreach(const auto item, itemListJob->changedItems()) {
                qDebug() << item.contentType() << item.data();
                auto itemUrl(item.url());
                auto itemFetchJob = new KDAV::DavItemFetchJob(itemUrl,item);
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

                cache.setEtag(item.url().toDisplayString(), item.etag());
            }
            cache.setEtag(QStringLiteral("invalid"),QStringLiteral("invalid"));
        }
        {
            qDebug() << "second run: (should be empty).";
            auto itemListJob = new KDAV::DavItemsListJob(collectionUrl, &cache);
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
}
