// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class DavCollectionModifyJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void modifySucceeds();
    void modifyServerError();
    void modifyResponseError();
    void modifyNoProperties();
    void removePropertySucceeds();
};
