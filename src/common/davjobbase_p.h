/*
    Copyright (c) 2014 Gregory Oestreicher <greg@kamago.net>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
