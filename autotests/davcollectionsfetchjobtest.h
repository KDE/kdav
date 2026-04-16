// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class DavCollectionsFetchJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    // Success cases
    void fetchCalDavCollections();
    void fetchCardDavCollections();
    void fetchGroupDavCollections();
    void calendarWithColor();

    // Error cases
    void principalFetchError();
    void collectionFetchError();
};
