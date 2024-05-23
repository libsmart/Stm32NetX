/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "X509.hpp"

UINT Stm32NetX::Secure::X509::certificateInitialize(const UCHAR *certificate_data, USHORT certificate_data_length,
                                                    UCHAR *raw_data_buffer, USHORT buffer_size,
                                                    const UCHAR *private_key_data, USHORT private_key_data_length,
                                                    UINT private_key_type) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32NetX::Secure::X509::certificateInitialize()\r\n");


    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/netx-duo-secure-tls/chapter4.md#nx_secure_x509_certificate_initialize
    const auto ret = nx_secure_x509_certificate_initialize(
        cert,
        const_cast<UCHAR *>(certificate_data),
        certificate_data_length,
        raw_data_buffer,
        buffer_size,
        private_key_data,
        private_key_data_length,
        private_key_type
    );
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("X509 certificate initialization failed. nx_secure_x509_certificate_initialize() = 0x%02x\r\n",
                         ret);
    }

    return ret;
}
