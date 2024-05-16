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
              byte_pool(byte_pool),
              bytePool(*byte_pool) {
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

            UINT ret = TX_SUCCESS;
            UCHAR *memPtr = nullptr;


            // Stm32ThreadX::BytePool bytePool(*byte_pool);

            // Allocate memory for the parser object
            ret = tx_byte_allocate(byte_pool, reinterpret_cast<void **>(&memPtr),
                                   sizeof(NetX),
                                   TX_NO_WAIT);
            if (ret != TX_SUCCESS) {
                Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                        ->printf("%lu: tx_byte_allocate() = 0x%02x", millis(), ret);
                assert_param(ret == TX_SUCCESS);
            }

            // Create NetX obejct
            NX = new(memPtr) NetX(byte_pool, &Stm32ItmLogger::logger);

            return TX_SUCCESS;
        }

    protected:
        TX_BYTE_POOL *byte_pool;
        Stm32ThreadX::BytePool bytePool;
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
}

#endif //LIBSMART_STM32NETX_STM32NETX_HPP
