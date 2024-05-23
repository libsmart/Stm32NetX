/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32NETX_SECURE_X509_HPP
#define LIBSMART_STM32NETX_SECURE_X509_HPP

#include "Loggable.hpp"
#include "nx_api.h"
#ifdef NX_SECURE_ENABLE
#include "nx_secure_x509.h"

namespace Stm32NetX {
    namespace Secure {
        class X509 : public Stm32ItmLogger::Loggable {
        public:
            explicit X509(NX_SECURE_X509_CERT *cert)
                : cert(cert) {
            }

            X509(NX_SECURE_X509_CERT *cert, Stm32ItmLogger::LoggerInterface *logger)
                : Loggable(logger),
                  cert(cert) {
            }

            UINT certificateInitialize(const UCHAR *certificate_data,
                                       USHORT certificate_data_length,
                                       UCHAR *raw_data_buffer,
                                       USHORT buffer_size,
                                       const UCHAR *private_key_data,
                                       USHORT private_key_data_length,
                                       UINT private_key_type);


            UINT certificateInitialize(const UCHAR *certificate_data,
                                       const USHORT certificate_data_length) {
                return certificateInitialize(certificate_data, certificate_data_length,
                                             nullptr, 0,
                                             nullptr, 0,
                                             NX_SECURE_X509_KEY_TYPE_NONE);
            }

            [[nodiscard]] NX_SECURE_X509_CERT *getCert() const { return cert; }

        private:
            NX_SECURE_X509_CERT *cert;
        };
    }
}
#endif
#endif //LIBSMART_STM32NETX_SECURE_X509_HPP
