// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class DavItemsFetchJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void fetchCardDavSingleItem();
    void fetchCardDavTwoItems();
    void fetchCardDavServerError();
    void fetchCardDavSkipsNonOkPropstat();
    void fetchCalDavSingleItem();
    void fetchFailsForProtocolWithoutMultiget();
};
