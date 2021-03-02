/*
    SPDX-FileCopyrightText: 2014 Gregory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVJOBBASE_P_H
#define KDAV_DAVJOBBASE_P_H

#include <QString>

#include <memory>

namespace KDAV
{
class DavJobBase;

class DavJobBasePrivate
{
public:
    virtual ~DavJobBasePrivate() = default;

    /**
     * Sets the latest response code received.
     *
     * Only really useful in case of error, though success codes can
     * also be set.
     *
     * @param code The code to set, should be a valid HTTP response code or zero.
     */
    void setLatestResponseCode(int code);

    void setJobErrorText(const QString &errorText);
    void setJobError(int jobErrorCode);
    void setErrorTextFromDavError();
    void setDavError(const Error &error);

    // forwarded protected KJob API, so we can use this from subclasses of this
    void setError(int errorCode);
    void setErrorText(const QString &errorText);
    void emitResult();

    DavJobBase *q_ptr = nullptr;
    int mLatestResponseCode = 0;
    int mJobErrorCode = 0;
    QString mInternalErrorText;
};

}

#endif
