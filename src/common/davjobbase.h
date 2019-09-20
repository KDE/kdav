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

#ifndef KDAV_DAVJOBBASE_H
#define KDAV_DAVJOBBASE_H

#include <memory>

#include "kpimkdav_export.h"

#include <KJob>

class DavJobBasePrivate;

namespace KDAV {
class Error;

/**
 * @short base class for the jobs used by the resource.
 */
class KPIMKDAV_EXPORT DavJobBase : public KJob
{
    Q_OBJECT

public:
    explicit DavJobBase(QObject *parent = nullptr);
    ~DavJobBase();

    /**
     * Get the latest response code.
     *
     * If no response code has been set then 0 will be returned, but will
     * be meaningless unless error() is non-zero. In that case this means
     * that the latest error was not at the HTTP level.
     */
    Q_REQUIRED_RESULT int latestResponseCode() const;

    /**
     * Check if the job can be retried later.
     *
     * This will return true for transient errors, i.e. if the response code
     * is either zero and error() is set or if the HTTP response code hints
     * at a temporary error.
     *
     * The HTTP response codes considered retryable are:
     *   - 401
     *   - 402
     *   - 407
     *   - 408
     *   - 423
     *   - 429
     *   - 501 to 504, inclusive
     *   - 507
     *   - 511
     */
    Q_REQUIRED_RESULT bool canRetryLater() const;

    /**
     * Check if the job failed because of a conflict
     */
    Q_REQUIRED_RESULT bool hasConflict() const;

    /**
     * Returns a instance of the KDAV:Error to be able to translate the error
     */
    Q_REQUIRED_RESULT Error davError() const;

protected:
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

    explicit DavJobBase(DavJobBasePrivate *dd, QObject *parent = nullptr);
    std::unique_ptr<DavJobBasePrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(DavJobBase)
};
}

#endif
