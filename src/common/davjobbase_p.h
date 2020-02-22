/*
    SPDX-FileCopyrightText: 2014 Gregory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVJOBBASE_P_H
#define KDAV_DAVJOBBASE_P_H

#include <QString>

#include <memory>

class DavJobBasePrivate
{
public:
    virtual ~DavJobBasePrivate() = default;

    int mLatestResponseCode = 0;
    int mJobErrorCode = 0;
    QString mInternalErrorText;
};

#endif
