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

        ULONG getInterfaceStatus();

        UINT interfaceStatusCheck(ULONG &actual_status);

        UINT interfaceStatusCheck(ULONG needed_status, ULONG wait_option);

        /**
         * Checks the interface status of the IP instance.
         *
         * @param needed_status The status that is required for the interface.
         * @param actual_status The actual status of the interface. This parameter is updated by the method.
         * @param wait_option The optional wait option for the status check.
         *
         * @return The status code of the status check. Possible values are:
         *         - NX_SUCCESS: The status check was successful.
         *         - Other error codes indicating failure.
         */
        UINT interfaceStatusCheck(ULONG needed_status, ULONG &actual_status, ULONG wait_option);

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
