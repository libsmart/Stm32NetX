/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32NETX_DNS_DNS_HPP
#define LIBSMART_STM32NETX_DNS_DNS_HPP

#include "Address.hpp"
#include "Loggable.hpp"
#include "Nameable.hpp"

namespace Stm32NetX {
    class Dns : protected NX_DNS, public Stm32ItmLogger::Loggable, public Stm32Common::Nameable {
    public:
        UINT create(NX_IP *ip_ptr, CHAR *domain_name);

        UINT create();

        UINT del();

        UINT serverAdd(NXD_ADDRESS *server_address);

        UINT hostByNameGet(CHAR *host_name,
                           NXD_ADDRESS *host_address_ptr,
                           ULONG wait_option,
                           UINT lookup_type);
    };
}
#endif
