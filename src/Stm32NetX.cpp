/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Stm32NetX.hpp"

using namespace Stm32NetX;

UINT NetX::createNetworkThread() {
    setStack(bytePool.allocate(LIBSMART_STM32NETX_NETX_THREAD_STACK_SIZE), LIBSMART_STM32NETX_NETX_THREAD_STACK_SIZE);

    // Create thread
    createThread();

    // Run begin() to start thread

    return TX_SUCCESS;
}

void NetX::begin() {
    resume();
}

void NetX::networkThread() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::NetX::networkThread()");

    ULONG actual_status;

    enum {
        LINK_UNKNOWN,
        LINK_DOWN,
        LINK_UP
    } linkState = LINK_UNKNOWN;

    enum {
        IP_UNKNOWN,
        IP_UNSET,
        IP_SET
    } ipState = IP_UNKNOWN;


    // Create packet pool service
    packetPool->setName("Stm32NetX::PacketPool");
    if (packetPool->create(LIBSMART_STM32NETX_PAYLOAD_SIZE, memPacketPool, LIBSMART_STM32NETX_PACKET_POOL_SIZE) == NX_SUCCESS) {
        flags.set(HAS_PACKET_POOL);
    }


    // Create ip instance service
    if (ipInstance->create() == NX_SUCCESS) {
        flags.set(HAS_IP_INSTANCE);
    }


#ifdef NX_ENABLE_DUAL_PACKET_POOL
    // Create packet pool service
    packetPoolAux->setName("Stm32NetX::PacketPoolAux");
    if (packetPoolAux->create(LIBSMART_STM32NETX_PAYLOAD_AUX_SIZE, memPacketPoolAux, LIBSMART_STM32NETX_PACKET_POOL_AUX_SIZE) == NX_SUCCESS) {
        flags.set(HAS_PACKET_POOL);
    }

    const auto ret = nx_ip_auxiliary_packet_pool_set(ipInstance, packetPoolAux);
    assert_param(ret == NX_SUCCESS);
#endif


    // Enable arp service
    if (arp != nullptr && arp->enable() == NX_SUCCESS) {
        flags.set(HAS_ARP_ENABLED);
    }


    // Enable icmp service
    if (icmp != nullptr && icmp->enable() == NX_SUCCESS) {
        flags.set(HAS_ICMP_ENABLED);
    }


    // Enable udp service
#ifdef NX_IP_UDP_ENABLED
    if (udp != nullptr && udp->enable() == NX_SUCCESS) {
        flags.set(HAS_UDP_ENABLED);
    }
#endif


    // Enable tcp service
#ifdef NX_IP_TCP_ENABLED
    if (tcp != nullptr && tcp->enable() == NX_SUCCESS) {
        flags.set(HAS_TCP_ENABLED);
    }
#endif


    // Create dhcp service
#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
    if (dhcp != nullptr && dhcp->create() == NX_SUCCESS) {
        flags.set(HAS_DHCP_ENABLED);
    }
#endif


    // Wait until ip instance and driver is initialized
    assert_param(ipInstance->interfaceStatusCheck(NX_IP_INITIALIZE_DONE, TX_WAIT_FOREVER) == NX_SUCCESS);


    /*
    // Enable interface link, if not already enabled
    // This happens,
    if(ipInstance->interfaceStatusCheck(NX_IP_INTERFACE_LINK_ENABLED, 0) != NX_SUCCESS) {
        // Enable link
        // If MCU was started without LAN cable connected, link is not up
        ipInstance->driverDirectCommand(NX_LINK_ENABLE, &actual_status);
    }
    */



    for (;;) {
        // HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);

        // Check for network link
        UINT ret = ipInstance->interfaceStatusCheck(NX_IP_LINK_ENABLED, TX_TIMER_TICKS_PER_SECOND / 10);
        if (ret != NX_SUCCESS) {
            if (linkState != LINK_DOWN) {
                log(Stm32ItmLogger::LoggerInterface::Severity::NOTICE)
                        ->println("Stm32NetX::NetX::networkThread(): LINK DOWN");
            }
            linkState = LINK_DOWN;
            ipState = IP_UNKNOWN;
            if (flags.isSet(HAS_LINK) || flags.isSet(HAS_IP)) {
                flags.clear(HAS_LINK | HAS_IP);
            }
            tx_thread_sleep(LIBSMART_STM32NETX_NETX_LINK_CHECK_INTERVAL);
            continue; // NO LINK, repeat
        }


        // Link detected
        if (linkState != LINK_UP) {
            log(Stm32ItmLogger::LoggerInterface::Severity::NOTICE)
                    ->println("Stm32NetX::NetX::networkThread(): LINK UP");
        }
        linkState = LINK_UP;
        flags.isSet(HAS_LINK) ? (void) 0 : (void) flags.set(HAS_LINK);
        // eventFlags.set(NetXEventFlags::Flags_t{NetXEventFlags::Flags::HAS_LINK}, Stm32ThreadX::EventFlags::setOption_t::OR);


        // Check for IP address
        ret = ipInstance->interfaceStatusCheck(NX_IP_ADDRESS_RESOLVED, TX_TIMER_TICKS_PER_SECOND / 10);
        if (ret == NX_SUCCESS) {
            // Connected and IP set
            if (ipState != IP_SET) {
                const ULONG ip = ipInstance->ipAddressGet();
                log(Stm32ItmLogger::LoggerInterface::Severity::NOTICE)
                        ->printf("Stm32NetX::NetX::networkThread(): IP SET (%lu.%lu.%lu.%lu/%d)\r\n",
                                 (ip >> 24) & 0xff,
                                 (ip >> 16) & 0xff,
                                 (ip >> 8) & 0xff,
                                 (ip >> 0) & 0xff,
                                 ipInstance->ipMaskCidrGet()
                        );
            }
            ipState = IP_SET;
            flags.isSet(HAS_IP) ? (void) 0 : (void) flags.set(HAS_IP);

            tx_thread_sleep(LIBSMART_STM32NETX_NETX_LINK_CHECK_INTERVAL);
        } else {
            // IP not set
            flags.isSet(HAS_IP) ? (void) flags.clear(HAS_IP) : (void) 0;

            if (ipState != IP_UNSET) {
                log(Stm32ItmLogger::LoggerInterface::Severity::NOTICE)
                        ->println("Stm32NetX::NetX::networkThread(): IP UNSET");
            }
            ipState = IP_UNSET;


            // Enable link
            // If MCU was started without LAN cable connected, link is not up
            ipInstance->driverDirectCommand(NX_LINK_ENABLE, &actual_status);


#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
            // Restart DHCP Client
            dhcp->stop();
            if (getConfig()->dhcp) {
                dhcp->reinitialize();
                dhcp->start();
            }
#endif

            // Set static ip
            if (!getConfig()->dhcp) {
                ipInstance->ipAddressSet(getConfig()->ip_address, getConfig()->network_mask);
                ipInstance->ipGatewayAddressSet(getConfig()->gateway_address);
            }

            tx_thread_sleep(LIBSMART_STM32NETX_NETX_DHCP_WAIT_TIME);
        }
    }
}

UINT NetX::waitForIpInstance() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::NetX::waitForIpInstance()");
    return flags.await(HAS_IP_INSTANCE);
}

UINT NetX::waitForPacketPool() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::NetX::waitForPacketPool()");
    return flags.await(HAS_PACKET_POOL);
}

UINT NetX::waitForLink() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::NetX::waitForLink()");
    return flags.await(HAS_LINK);
}

UINT NetX::waitForIp() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::NetX::waitForIp()");
    return flags.await(HAS_IP);
}
