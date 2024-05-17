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
    inline char memPacketpool[LIBSMART_STM32NETX_PACKET_POOL_SIZE] __attribute__((section(".data")));
#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
    inline char memDhcp[sizeof(Dhcp)] __attribute__((section(".data")));
#endif
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
            log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->println("Stm32NetX::NetX::NetX()");
            bytePool.setLogger(getLogger());

            packetPool = new(bytePool.allocate(sizeof(PacketPool))) PacketPool(*this);
            packetPool->create();

            ipInstance = new(bytePool.allocate(sizeof(IpInstance))) IpInstance(*this, *packetPool);
            ipInstance->create();

            arp = new(bytePool.allocate(sizeof(Arp))) Arp(*this, *ipInstance);
            arp->enable();

            icmp = new(bytePool.allocate(sizeof(Icmp))) Icmp(*this, *ipInstance);
            icmp->enable();

#ifdef NX_IP_UDP_ENABLED
            udp = new(bytePool.allocate(sizeof(Udp))) Udp(*this, *ipInstance);
            udp->enable();
#endif

#ifdef NX_IP_TCP_ENABLED
            tcp = new(bytePool.allocate(sizeof(Tcp))) Tcp(*this, *ipInstance);
            tcp->enable();
#endif

#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
            // Don't load into ccm ram, as dhcp has its own packet pool
            // and ethernet is using DMA
            // dhcp = new(bytePool.allocate(sizeof(Dhcp))) Dhcp(*this, *ipInstance);
            dhcp = new(memDhcp) Dhcp(*this, *ipInstance);
            dhcp->create();
#endif

            createNetworkThread();
        }

        UINT createNetworkThread();


        [[noreturn]] void networkThread();


        static UINT setup(TX_BYTE_POOL *byte_pool) {
            Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->println("Stm32NetX::NetX::setup()");

            // Initialize NetX Duo System
            nx_system_initialize();

            // NetX requires a BytePool, but we also want to allocate the memory for NetX
            // with this BytePool.
            bytePool.setLogger(&Stm32ItmLogger::logger);
            bytePool.setBytePoolStruct(byte_pool);

            // Create NetX obejct
            NX = new(bytePool.allocate(sizeof(NetX))) NetX(byte_pool, &Stm32ItmLogger::logger);
            // NX = new NetX(byte_pool, &Stm32ItmLogger::logger);

            return TX_SUCCESS;
        }

    protected:
        TX_BYTE_POOL *byte_pool;
        static Stm32ThreadX::BytePool bytePool;
        PacketPool *packetPool;
        IpInstance *ipInstance;
        Arp *arp;
        Icmp *icmp;
#ifdef NX_IP_UDP_ENABLED
        Udp *udp = {};
#endif
#ifdef NX_IP_TCP_ENABLED
        Tcp *tcp = {};
#endif
#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
        Dhcp *dhcp = {};
#endif
    };

    inline Stm32ThreadX::BytePool NetX::bytePool(nullptr);
}

#endif //LIBSMART_STM32NETX_STM32NETX_HPP
