/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32NETX_IPINSTANCE_HPP
#define LIBSMART_STM32NETX_IPINSTANCE_HPP

#include <libsmart_config.hpp>
#include "Loggable.hpp"
#include "nx_api.h"

namespace Stm32NetX {
    class NetX;
    class PacketPool;

    class IpInstance : public Stm32ItmLogger::Loggable, public NX_IP {
    public:
        IpInstance() = delete;

        IpInstance(NetX &NX, PacketPool &packetPool);

        UINT create();

        UINT interfaceStatusCheck(ULONG needed_status, ULONG wait_option);

        UINT driverDirectCommand(UINT command, ULONG *return_value_ptr);

        UINT ipAddressGet(ULONG *ip_address, ULONG *network_mask);

        ULONG ipAddressGet();

        ULONG ipMaskGet();

        uint8_t ipMaskCidrGet();

        UINT ipAddressSet(ULONG ip_address, ULONG network_mask);

        UINT ipGatewayAddressClear();

        UINT ipGatewayAddressGet(ULONG *ip_address);

        ULONG ipGatewayAddressGet();

        UINT ipGatewayAddressSet(ULONG ip_address);

    protected:
        NetX &NX;
        PacketPool &packetPool;
    };
}

#endif //LIBSMART_STM32NETX_IPINSTANCE_HPP
