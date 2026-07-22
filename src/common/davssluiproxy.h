/*
    SPDX-FileCopyrightText: 2026 Kevin Ottens <kevin.ottens@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "kdav_export.h"

#include <memory>

class KSslErrorUiData;

namespace KDAV
{
/*!
 * \class KDAV::DavSslUiProxy
 * \inmodule KDAV
 * \inheaderfile KDAV/DavSslUiProxy
 *
 * \brief Interface to display SSL errors and requesting user input.
 * \since 6.29
 */
class KDAV_EXPORT DavSslUiProxy
{
public:
    /*!
     * Set an implementation for the DavSslUiProxy interface.
     *
     * If none has been positioned, any SSL error will be automatically
     * rejected and fail the corresponding job. Position one to avoid
     * this behavior or be able to ask the user what to do.
     */
    static void setDefaultProxy(std::unique_ptr<DavSslUiProxy> &&proxy);

    virtual ~DavSslUiProxy();

    /*!
     * Show an SSL error and ask the user whether it should be ignored or not.
     * The recommended KDE UI is the following:
     * \code
     * #include <KIO/SslUi>
     * class UiProxy: public DavSslUiProxy
     * {
     * public:
     *     bool ignoreSslError(const KSslErrorUiData &errorData)
     *     {
     *         if (KIO::SslUi::askIgnoreSslErrors(errorData)) {
     *             return true;
     *         } else {
     *            return false;
     *         }
     *     }
     * };
     * [...]
     * auto proxy = std::make_unique<UiProxy>();
     * DavSslUiProxy::setDefaultProxy(std::move(proxy));
     * \endcode
     *
     * \a errorData contains details about the error.
     *
     * Returns \c true if the error can be ignored
     */
    virtual bool ignoreSslError(const KSslErrorUiData &errorData) = 0;
};

}
