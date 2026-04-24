// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class DavPrincipalSearchJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void searchByDisplayName();
    void searchByEmailAddress();
    void propfindHttpError();
    void propfindEmptyMultistatus();
    void propfindNoPropstat200();
    void reportHttpError();
    void multipleCollectionSets();
};
