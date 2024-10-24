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
#include "EventFlags.hpp"
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
    inline ULONG memPacketPool[LIBSMART_STM32NETX_PACKET_POOL_SIZE / sizeof(ULONG)] __attribute__((section(".data")));
#ifdef NX_ENABLE_DUAL_PACKET_POOL
    inline ULONG memPacketPoolAux[LIBSMART_STM32NETX_PACKET_POOL_AUX_SIZE / sizeof(ULONG)] __attribute__((section(".data")));
#endif
#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
    inline char memDhcp[sizeof(Dhcp)] __attribute__((section(".data")));
#endif


    class NetXConfig {
    public:
        const char *hostname = LIBSMART_STM32NETX_DEFAULT_HOSTNAME;
        bool dhcp = LIBSMART_STM32NETX_DEFAULT_DHCP;
        ULONG ip_address = LIBSMART_STM32NETX_DEFAULT_IP_ADDRESS;
        ULONG network_mask = LIBSMART_STM32NETX_DEFAULT_NETMASK;
        ULONG gateway_address = LIBSMART_STM32NETX_DEFAULT_GATEWAY_ADDRESS;
    };


    class NetX : public Stm32ItmLogger::Loggable, public Stm32ThreadX::Thread {
        friend PacketPool;
        friend IpInstance;
        friend Arp;

        using Flags = enum {
            NONE = 0,
            HAS_LINK = 1 << 0,
            HAS_IP = 1 << 1,
            HAS_IP_INSTANCE = 1 << 2,
            HAS_PACKET_POOL = 1 << 3,
            HAS_ARP_ENABLED = 1 << 4,
            HAS_ICMP_ENABLED = 1 << 5,
            HAS_UDP_ENABLED = 1 << 6,
            HAS_TCP_ENABLED = 1 << 7,
            HAS_DHCP_ENABLED = 1 << 8,
            THE_END = 1 << 31
        };

    public:
        explicit NetX(TX_BYTE_POOL *byte_pool)
            : NetX(byte_pool, getLogger()) { ; }

        NetX(TX_BYTE_POOL *byte_pool, Stm32ItmLogger::LoggerInterface *logger)
            : Loggable(logger),
              Thread(Stm32ThreadX::BOUNCE(NetX, networkThread), reinterpret_cast<ULONG>(this), priority(),
                     "Stm32NetX::NetX::networkThread()"),
              byte_pool(byte_pool) {
            log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->println("Stm32NetX::NetX::NetX()");
            bytePool.setLogger(getLogger());

            // assert_param(flags.create() == TX_SUCCESS);

            packetPool = new(bytePool.allocate(sizeof(PacketPool))) PacketPool(*this);

            packetPoolAux = new(bytePool.allocate(sizeof(PacketPool))) PacketPool(*this);

            ipInstance = new(bytePool.allocate(sizeof(IpInstance))) IpInstance(*this, *packetPool);

            arp = new(bytePool.allocate(sizeof(Arp))) Arp(*this, *ipInstance);

            icmp = new(bytePool.allocate(sizeof(Icmp))) Icmp(*this, *ipInstance);

#ifdef NX_IP_UDP_ENABLED
            udp = new(bytePool.allocate(sizeof(Udp))) Udp(*this, *ipInstance);
#endif

#ifdef NX_IP_TCP_ENABLED
            tcp = new(bytePool.allocate(sizeof(Tcp))) Tcp(*this, *ipInstance);
#endif

#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
            // Don't load into ccm ram, as dhcp has its own packet pool
            // and ethernet is using DMA
            // dhcp = new(bytePool.allocate(sizeof(Dhcp))) Dhcp(*this, *ipInstance);
            dhcp = new(memDhcp) Dhcp(*this, *ipInstance);
#endif

            createNetworkThread();
        }


        /**
         * @brief Creates the network thread.
         *
         * This method creates the network thread and initializes the required resources.
         *
         * @return TX_SUCCESS if the network thread is created successfully,
         *         or another error code if the creation fails.
         */
        UINT createNetworkThread();


        /**
         * @brief Begins the network operation.
         *
         * This method starts the network operation and resumes the network thread.
         * It is called to initialize the network stack and prepare it for sending and receiving data.
         *
         * @note This method should be called after setting up the required configurations.
         */
        void begin();

        void restart();

        [[noreturn]] void networkThread();


        static Stm32ThreadX::BytePool *getBytePool() { return &bytePool; }

        [[nodiscard]] virtual PacketPool *getPacketPool() const {
            return packetPool;
        }

        [[nodiscard]] virtual IpInstance *getIpInstance() const {
            return ipInstance;
        }

        UINT waitForIpInstance();

        UINT waitForPacketPool();

        UINT waitForLink();

        UINT waitForIp();

        bool isFlagSet(ULONG requestedFlags) { return flags.isSet(requestedFlags); }
        bool isIpSet() { return isFlagSet(HAS_IP); }

        NetXConfig *getConfig() { return &nxConfig; };

    protected:
        NetXConfig nxConfig;
        Stm32ThreadX::EventFlags flags{"Stm32NetX::NetX::flags", getLogger()};
        TX_BYTE_POOL *byte_pool;
        static Stm32ThreadX::BytePool bytePool;
        PacketPool *packetPool;
        PacketPool *packetPoolAux;
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

    public:
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
    };

    inline Stm32ThreadX::BytePool NetX::bytePool;
}

#endif //LIBSMART_STM32NETX_STM32NETX_HPP
