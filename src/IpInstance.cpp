/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "IpInstance.hpp"
#include "nx_stm32_eth_driver.h"
#include "Stm32NetX.hpp"

using namespace Stm32NetX;

#define STATIC_IP                               IP_ADDRESS(10,82,3,199) * 0
#define STATIC_IP_MASK                          IP_ADDRESS(255,255,255,0) * 0
#define DHCP_IP                                 1
#define DEFAULT_PRIORITY                        10

IpInstance::IpInstance(NetX &NX, PacketPool &packetPool) : NX_IP(), NX(NX), packetPool(packetPool) {
    setLogger(NX.getLogger());
}

UINT IpInstance::create() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::IpInstance::create()");

    UINT ret = NX_SUCCESS;
    UCHAR *memPtr = nullptr;
    CHAR ipInstance_name[] = "Stm32NetX::IpInstance";

    // Allocate the memory for ip instance
    ret = tx_byte_allocate(NX.byte_pool, reinterpret_cast<void **>(&memPtr),
                           2 * LIBSMART_STM32NETX_DEFAULT_MEMORY_SIZE, TX_NO_WAIT);
    if (ret != TX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("IP instance allocation failed. tx_byte_allocate() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }


    // Create the IP instance
    ret = nx_ip_create(this, ipInstance_name, STATIC_IP, STATIC_IP_MASK, &NX.packetPool,
                       nx_stm32_eth_driver,
                       memPtr, 2 * LIBSMART_STM32NETX_DEFAULT_MEMORY_SIZE, DEFAULT_PRIORITY);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("IP instance creation failed. nx_ip_create() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }


    return TX_SUCCESS;
}

UINT IpInstance::interfaceStatusCheck(ULONG needed_status, ULONG wait_option) {
    ULONG actual_status;
    const auto ret = nx_ip_interface_status_check(this, 0, needed_status,
                                                  &actual_status, wait_option);
    if (ret != NX_SUCCESS && ret != NX_NOT_SUCCESSFUL) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Interface status check failed. nx_ip_interface_status_check() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT IpInstance::ipAddressGet(ULONG *ip_address, ULONG *network_mask) {
    const auto ret = nx_ip_address_get(this, ip_address, network_mask);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("IP address get failed. nx_ip_address_get() = 0x%02x\r\n", ret);
    }
    return ret;
}

ULONG IpInstance::ipAddressGet() {
    ULONG ip;
    ULONG mask;
    return ipAddressGet(&ip, &mask) == NX_SUCCESS ? ip : 0;
}

ULONG IpInstance::ipMaskGet() {
    ULONG ip;
    ULONG mask;
    return ipAddressGet(&ip, &mask) == NX_SUCCESS ? mask : 0;
}

uint8_t IpInstance::ipMaskCidrGet() {
    auto mask = ipMaskGet();
    uint8_t count = 0;
    while (mask) {
        mask &= (mask - 1); // clear the least significant bit set
        count++;
    }
    return count;
}
