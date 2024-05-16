/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Arp.hpp"
#include "Stm32NetX.hpp"
#include "IpInstance.hpp"

using namespace Stm32NetX;

Arp::Arp(NetX &NX, IpInstance &ipInstance) : NX(NX), ipInstance(ipInstance) {
    setLogger(NX.getLogger());
}

UINT Arp::enable() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::Arp::enable()");

    UINT ret = NX_SUCCESS;
    UCHAR *memPtr = nullptr;

    // Allocate the arp cache memory
    ret = tx_byte_allocate(NX.byte_pool, reinterpret_cast<void **>(&memPtr),
                           LIBSMART_STM32NETX_DEFAULT_MEMORY_SIZE, TX_NO_WAIT);
    if (ret != TX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("ARP cache memory allocation failed. tx_byte_allocate() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }


    //  Enable the ARP protocol and provide the ARP cache size for the IP instance
    ret = nx_arp_enable(&ipInstance, static_cast<void *>(memPtr), LIBSMART_STM32NETX_DEFAULT_MEMORY_SIZE);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("ARP enable failed. nx_arp_enable() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }


    return TX_SUCCESS;
}
