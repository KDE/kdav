/*
    SPDX-FileCopyrightText: 2014 Gregory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davjobbase.h"
#include "davjobbase_p.h"

#include "daverror.h"

using namespace KDAV;

DavJobBase::DavJobBase(QObject *parent)
    : KJob(parent)
    , d_ptr(new DavJobBasePrivate())
{
}

DavJobBase::DavJobBase(DavJobBasePrivate *dd, QObject *parent)
    : KJob(parent)
    , d_ptr(dd)
{
}

DavJobBase::~DavJobBase() = default;

int DavJobBase::latestResponseCode() const
{
    return d_ptr->mLatestResponseCode;
}

bool DavJobBase::canRetryLater() const
{
    bool ret = false;

    // Be explicit and readable by splitting the if/else if clauses

    if (latestResponseCode() == 0 && error()) {
        // Likely a timeout or a connection failure.
        ret = true;
    } else if (latestResponseCode() == 401) {
        // Authentication required
        ret = true;
    } else if (latestResponseCode() == 402) {
        // Payment required
        ret = true;
    } else if (latestResponseCode() == 407) {
        // Proxy authentication required
        ret = true;
    } else if (latestResponseCode() == 408) {
        // Request timeout
        ret = true;
    } else if (latestResponseCode() == 423) {
        // Locked
        ret = true;
    } else if (latestResponseCode() == 429) {
        // Too many requests
        ret = true;
    } else if (latestResponseCode() >= 501 && latestResponseCode() <= 504) {
        // Various server-side errors
        ret = true;
    } else if (latestResponseCode() == 507) {
        // Insufficient storage
        ret = true;
    } else if (latestResponseCode() == 511) {
        // Network authentication required
        ret = true;
    }

    return ret;
}

bool DavJobBase::hasConflict() const
{
    return latestResponseCode() == 412;
}

void DavJobBase::setLatestResponseCode(int code)
{
    d_ptr->mLatestResponseCode = code;
}

Error DavJobBase::davError() const
{
    return Error(static_cast<KDAV::ErrorNumber>(error()), d_ptr->mLatestResponseCode, d_ptr->mInternalErrorText, d_ptr->mJobErrorCode);
}

void DavJobBase::setJobErrorText(const QString &errorText)
{
    d_ptr->mInternalErrorText = errorText;
}

void DavJobBase::setJobError(int jobErrorCode)
{
    d_ptr->mJobErrorCode = jobErrorCode;
}

void DavJobBase::setErrorTextFromDavError()
{
    setErrorText(davError().errorText());
}

void DavJobBase::setDavError(const Error &error)
{
    setError(error.errorNumber());
    setLatestResponseCode(error.responseCode());
    setJobErrorText(error.internalErrorText());
    setJobError(error.jobErrorCode());
}
