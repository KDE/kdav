/*
    SPDX-FileCopyrightText: 2014 Gregory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVJOBBASE_H
#define KDAV_DAVJOBBASE_H

#include "kdav_export.h"

#include <KJob>

#include <memory>

namespace KDAV
{
class DavJobBasePrivate;
class Error;

/*!
 * \class KDAV::DavJobBase
 * \inheaderfile KDAV/DavJobBase
 * \inmodule KDAV
 *
 * \brief base class for the jobs used by the resource.
 */
class KDAV_EXPORT DavJobBase : public KJob
{
    Q_OBJECT

public:
    explicit DavJobBase(QObject *parent = nullptr);
    ~DavJobBase() override;

    /*!
     * Get the latest response code.
     *
     * If no response code has been set then 0 will be returned, but will
     * be meaningless unless error() is non-zero. In that case this means
     * that the latest error was not at the HTTP level.
     */
    Q_REQUIRED_RESULT int latestResponseCode() const;

    /*!
     * Check if the job can be retried later.
     *
     * This will return true for transient errors, i.e. if the response code
     * is either zero and error() is set or if the HTTP response code hints
     * at a temporary error.
     *
     * The HTTP response codes considered retryable are:
     * \list
     * \li 401
     * \li 402
     * \li 407
     * \li 408
     * \li 423
     * \li 429
     * \li 501 to 504, inclusive
     * \li 507
     * \li 511
     * \endlist
     */
    Q_REQUIRED_RESULT bool canRetryLater() const;

    /*!
     * Check if the job failed because of a conflict
     */
    Q_REQUIRED_RESULT bool hasConflict() const;

    /*!
     * Returns a instance of the KDAV:Error to be able to translate the error
     */
    Q_REQUIRED_RESULT Error davError() const;

protected:
    Q_DECL_HIDDEN explicit DavJobBase(DavJobBasePrivate *dd, QObject *parent = nullptr);
    std::unique_ptr<DavJobBasePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(DavJobBase)
};
}

#endif
