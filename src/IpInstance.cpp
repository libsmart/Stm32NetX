/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "IpInstance.hpp"
#include "nx_stm32_eth_driver.h"
#include "Stm32NetX.hpp"

using namespace Stm32NetX;

// #define STATIC_IP                               IP_ADDRESS(10,82,3,199) * 0
// #define STATIC_IP_MASK                          IP_ADDRESS(255,255,255,0) * 0
// #define DHCP_IP                                 1
// #define DEFAULT_PRIORITY                        10

IpInstance::IpInstance(NetX &NX, PacketPool &packetPool) : NX_IP(), NX(NX), packetPool(packetPool) {
    setLogger(NX.getLogger());
}

UINT IpInstance::create() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::IpInstance::create()");


    static CHAR name[] = "Stm32NetX::IpInstance";

    // Create the IP instance
    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_ip_create
    const auto ret = nx_ip_create(this, name, NX.getConfig()->ip_address * 0, NX.getConfig()->network_mask * 0,
                                  &packetPool,
                                  nx_stm32_eth_driver,
                                  NX.bytePool.allocate(2 * LIBSMART_STM32NETX_DEFAULT_MEMORY_SIZE),
                                  // ipinstance,
                                  2 * LIBSMART_STM32NETX_DEFAULT_MEMORY_SIZE, LIBSMART_STM32NETX_DEFAULT_PRIORITY);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("IP instance creation failed. nx_ip_create() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }

    return ret;
}

ULONG IpInstance::getInterfaceStatus() {
    ULONG actual_status = 0;
    interfaceStatusCheck(actual_status);
    return actual_status;
}

UINT IpInstance::interfaceStatusCheck(ULONG &actual_status) {
    return interfaceStatusCheck(0, actual_status, 0);
}

UINT IpInstance::interfaceStatusCheck(const ULONG needed_status, const ULONG wait_option) {
    ULONG actual_status = 0;
    return interfaceStatusCheck(needed_status, actual_status, wait_option);
}

UINT IpInstance::interfaceStatusCheck(const ULONG needed_status, ULONG &actual_status, const ULONG wait_option) {
    // log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
    // ->println("Stm32NetX::IpInstance::interfaceStatusCheck()");

    // ULONG actual_status;
    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_ip_interface_status_check
    const auto ret = nx_ip_interface_status_check(this, 0, needed_status,
                                                  &actual_status, wait_option);
    if (ret != NX_SUCCESS && ret != NX_NOT_SUCCESSFUL) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Interface status check failed. nx_ip_interface_status_check() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT IpInstance::driverDirectCommand(const UINT command, ULONG *return_value_ptr) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::IpInstance::driverDirectCommand()");

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_ip_driver_direct_command
    const auto ret = nx_ip_driver_direct_command(this, command, return_value_ptr);
    if (ret != NX_SUCCESS && ret != NX_ALREADY_ENABLED) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Direct driver command failed. nx_ip_driver_direct_command() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT IpInstance::ipAddressGet(ULONG *ip_address, ULONG *network_mask) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::IpInstance::ipAddressGet()");

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_ip_address_get
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

UINT IpInstance::ipAddressSet(const ULONG ip_address, const ULONG network_mask) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32NetX::IpInstance::ipAddressSet(%lu.%lu.%lu.%lu, %lu.%lu.%lu.%lu)\r\n",
                     (ip_address >> 24) & 0xff,
                     (ip_address >> 16) & 0xff,
                     (ip_address >> 8) & 0xff,
                     (ip_address >> 0) & 0xff,
                     (network_mask >> 24) & 0xff,
                     (network_mask >> 16) & 0xff,
                     (network_mask >> 8) & 0xff,
                     (network_mask >> 0) & 0xff
            );

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_ip_address_set
    const auto ret = nx_ip_address_set(this, ip_address, network_mask);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("IP address set failed. nx_ip_address_set() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT IpInstance::ipGatewayAddressClear() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::IpInstance::ipGatewayAddressClear()");

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_ip_gateway_address_clear
    const auto ret = nx_ip_gateway_address_clear(this);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("IP gateway address clear failed. nx_ip_gateway_address_clear() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT IpInstance::ipGatewayAddressGet(ULONG *ip_address) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::IpInstance::ipGatewayAddressGet()");

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_ip_gateway_address_get
    const auto ret = nx_ip_gateway_address_get(this, ip_address);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("IP gateway address get failed. nx_ip_gateway_address_get() = 0x%02x\r\n", ret);
    }
    return ret;
}

ULONG IpInstance::ipGatewayAddressGet() {
    ULONG ip_address;
    if (ipGatewayAddressGet(&ip_address) != NX_SUCCESS) {
        return 0;
    }
    return ip_address;
}

UINT IpInstance::ipGatewayAddressSet(ULONG ip_address) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32NetX::IpInstance::ipGatewayAddressSet(%lu.%lu.%lu.%lu)\r\n",
                     (ip_address >> 24) & 0xff,
                     (ip_address >> 16) & 0xff,
                     (ip_address >> 8) & 0xff,
                     (ip_address >> 0) & 0xff
            );

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_ip_gateway_address_set
    const auto ret = nx_ip_gateway_address_set(this, ip_address);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("IP gateway address set failed. nx_ip_gateway_address_set() = 0x%02x\r\n", ret);
    }
    return ret;
}
