// SPDX-FileCopyrightText: 2026 David Faure <faure@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class DavItemCreateJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void createItem_data();
    void createItem();
};
