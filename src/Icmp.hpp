/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32NETX_ICMP_HPP
#define LIBSMART_STM32NETX_ICMP_HPP

#include <libsmart_config.hpp>
#include "Loggable.hpp"
#include "nx_api.h"

namespace Stm32NetX {
    class NetX;
    class IpInstance;

    class Icmp : public Stm32ItmLogger::Loggable {
    public:
        Icmp() = delete;

        Icmp(NetX &NX, IpInstance &ipInstance);

        UINT enable();

    protected:
        NetX &NX;
        IpInstance &ipInstance;
    };
}

#endif //LIBSMART_STM32NETX_ICMP_HPP
