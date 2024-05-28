/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32NETX_DHCP_HPP
#define LIBSMART_STM32NETX_DHCP_HPP

#include <libsmart_config.hpp>


#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
#include "Loggable.hpp"
#include "nx_api.h"
#include "nxd_dhcp_client.h"

namespace Stm32NetX {
    class NetX;
    class IpInstance;

    class Dhcp : public Stm32ItmLogger::Loggable, public NX_DHCP {
    public:
        Dhcp() = delete;

        Dhcp(NetX &NX, IpInstance &ipInstance);

        UINT create();
        UINT start();
        UINT stop();
        UINT reinitialize();

        const char *getClientName();

    protected:
        NetX &NX;
        IpInstance &ipInstance;
    };
}

#endif
#endif //LIBSMART_STM32NETX_DHCP_HPP
