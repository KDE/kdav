/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DAVITEM_TEST_H
#define DAVITEM_TEST_H

#include <QObject>

class DavItemTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void createEmpty();
    void storeTest();
    void setTest();
    void copyTest();

    void serializeTest();
};

#endif
