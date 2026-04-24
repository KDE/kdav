// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class DavCollectionDeleteJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void deleteSucceeds();
    void deleteServerError();
};
