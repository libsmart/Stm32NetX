/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32NETX_STM32NETX_HPP
#define LIBSMART_STM32NETX_STM32NETX_HPP

#include <libsmart_config.hpp>
#include "Stm32ThreadX.hpp"
#include "Arp.hpp"
#include "BytePool.hpp"
#include "Dhcp.hpp"
#include "Icmp.hpp"
#include "Tcp.hpp"
#include "Udp.hpp"
#include "IpInstance.hpp"
#include "PacketPool.hpp"
#include "globals.hpp"
#include "Helper.hpp"
#include "Loggable.hpp"
#include "Thread.hpp"
#include "nx_api.h"
#include "tx_api.h"

namespace Stm32NetX {
    class NetX;
    inline NetX *NX = {};

    class NetX : public Stm32ItmLogger::Loggable, public Stm32ThreadX::Thread {
        friend PacketPool;
        friend IpInstance;
        friend Arp;

    public:
        explicit NetX(TX_BYTE_POOL *byte_pool)
            : NetX(byte_pool, getLogger()) { ; }

        explicit NetX(TX_BYTE_POOL *byte_pool, Stm32ItmLogger::LoggerInterface *logger)
            : Loggable(logger),
              Thread(Stm32ThreadX::BOUNCE(NetX, networkThread), reinterpret_cast<ULONG>(this), priority(),
                     "Stm32NetX::NetX"),
              byte_pool(byte_pool) {
            bytePool.setLogger(getLogger());
            packetPool.create();
            ipInstance.create();
            arp.enable();
            icmp.enable();
            udp.enable();
            tcp.enable();
#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
            dhcp.create();
#endif

            createNetworkThread();
        }

        UINT createNetworkThread();


        [[noreturn]] void networkThread();


        static UINT setup(TX_BYTE_POOL *byte_pool) {
            Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->println("Stm32NetX::NetX::setup()");

            // Initialize NetX Duo System
            // nx_system_initialize();

            // NetX requires a BytePool, but we also want to allocate the memory for NetX
            // with this BytePool.
            bytePool.setLogger(&Stm32ItmLogger::logger);
            bytePool.setBytePoolStruct(byte_pool);

            // Create NetX obejct
            NX = new(bytePool.allocate(sizeof(NetX))) NetX(byte_pool, &Stm32ItmLogger::logger);

            return TX_SUCCESS;
        }

    protected:
        TX_BYTE_POOL *byte_pool;
        static Stm32ThreadX::BytePool bytePool;
        PacketPool packetPool{*this};
        IpInstance ipInstance{*this, packetPool};
        Arp arp{*this, ipInstance};
        Icmp icmp{*this, ipInstance};
        Udp udp{*this, ipInstance};
        Tcp tcp{*this, ipInstance};
#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
        Dhcp dhcp{*this, ipInstance};
#endif
    };

    inline Stm32ThreadX::BytePool NetX::bytePool(nullptr);
}

#endif //LIBSMART_STM32NETX_STM32NETX_HPP
