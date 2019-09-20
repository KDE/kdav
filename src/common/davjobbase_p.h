/*
    Copyright (c) 2014 Gregory Oestreicher <greg@kamago.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

template <typename T>
static inline typename std::unique_ptr<T>::pointer qGetPtrHelper(const std::unique_ptr<T> &p)
{
    return p.get();
}

#endif
