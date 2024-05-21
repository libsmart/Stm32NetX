/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32NETX_ADDRESS_HPP
#define LIBSMART_STM32NETX_ADDRESS_HPP

#include "Loggable.hpp"
#include "nx_api.h"

namespace Stm32NetX {
    class Address : public Stm32ItmLogger::Loggable, public NXD_ADDRESS {
    public:
        Address() = default;

        explicit Address(Stm32ItmLogger::LoggerInterface *logger)
            : Loggable(logger) {
        }
    };
}
#endif //LIBSMART_STM32NETX_ADDRESS_HPP
