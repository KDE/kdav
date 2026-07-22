/*
    SPDX-FileCopyrightText: 2026 Kevin Ottens <kevin.ottens@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "kdav_export.h"

#include <memory>

#include <QList>

class QNetworkReply;
class QSslError;

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
     * #include <KSslErrorUiData>
     * #include <KIO/SslUi>
     * class UiProxy: public DavSslUiProxy
     * {
     * public:
     *     bool ignoreSslErrors(QNetworkReply *reply, const QList<QSslError> &sslErrors) override
     *     {
     *         KSslErrorUiData errorData(reply, sslErrors);
     *         if (KIO::SslUi::askIgnoreSslErrors(errorData)) {
     *             return true;
     *         } else {
     *             return false;
     *         }
     *     }
     * };
     * [...]
     * auto proxy = std::make_unique<UiProxy>();
     * DavSslUiProxy::setDefaultProxy(std::move(proxy));
     * \endcode
     *
     * \a reply is the request which got errors
     * \a sslErrors contains details about the encountered SSL errors.
     *
     * Returns \c true if the error can be ignored
     */
    virtual bool ignoreSslErrors(QNetworkReply *reply, const QList<QSslError> &sslErrors) = 0;
};

}
