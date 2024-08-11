/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32NETX_PACKETPOOL_HPP
#define LIBSMART_STM32NETX_PACKETPOOL_HPP

#include <libsmart_config.hpp>
#include "Loggable.hpp"
#include "Nameable.hpp"
#include "nx_api.h"

namespace Stm32NetX {
    class NetX;

    class PacketPool : public Stm32Common::Nameable, public Stm32ItmLogger::Loggable, public NX_PACKET_POOL {
    public:
        PacketPool() = delete;

        explicit PacketPool(NetX &NX);

        // UINT create();
        UINT create(ULONG payload_size, ULONG *packetPoolMemory, size_t packetPoolMemorySize);

    protected:
        NetX &NX;
    };
}

#endif //LIBSMART_STM32NETX_PACKETPOOL_HPP
