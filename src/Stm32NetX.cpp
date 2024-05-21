/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Stm32NetX.hpp"

using namespace Stm32NetX;

UINT NetX::createNetworkThread() {
    setStack(bytePool.allocate(LIBSMART_STM32NETX_NETX_THREAD_STACK_SIZE), LIBSMART_STM32NETX_NETX_THREAD_STACK_SIZE);

    // Start thread
    createThread();
    resume();

    return TX_SUCCESS;
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

    // Wait until hardware is ready
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * 3);

    // for(;;) {
    // tx_thread_sleep(1);
    // }

    for (;;) {
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);

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

            // Restart DHCP Client
#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
            dhcp->stop();
            dhcp->reinitialize();
            dhcp->start();
#endif
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
