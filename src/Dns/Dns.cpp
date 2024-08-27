/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#include <libsmart_config.hpp>
#ifdef LIBSMART_STM32NETX_ENABLE_DNS

#include "Dns.hpp"
#include "Stm32NetX.hpp"

using namespace Stm32NetX;


UINT Dns::create(NX_IP *ip_ptr, CHAR *domain_name) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::Dns::create()");

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/netx-duo-dns/chapter3.md#nx_dns_create
    const auto ret = nx_dns_create(this, ip_ptr, (UCHAR *) domain_name);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Stm32NetX::Dns: nx_dns_create() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT Dns::create() {
    return create(NX->getIpInstance(), getNameNonConst());
}

UINT Dns::del() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::Dns::del()");

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/netx-duo-dns/chapter3.md#nx_dns_delete
    const auto ret = nx_dns_delete(this);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Stm32NetX::Dns: nx_dns_delete() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT Dns::serverAdd(NXD_ADDRESS *server_address) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::Dns::serverAdd()");

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/netx-duo-dns/chapter3.md#nxd_dns_server_add
    const auto ret = nxd_dns_server_add(this, server_address);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Stm32NetX::Dns: nxd_dns_server_add() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT Dns::hostByNameGet(CHAR *host_name, NXD_ADDRESS *host_address_ptr, ULONG wait_option, UINT lookup_type) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::Dns::hostByNameGet()");

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/netx-duo-dns/chapter3.md#nxd_dns_host_by_name_get
    const auto ret = nxd_dns_host_by_name_get(this, (UCHAR *) host_name, host_address_ptr, wait_option, lookup_type);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Stm32NetX::Dns: nxd_dns_host_by_name_get() = 0x%02x\r\n", ret);
    }
    return ret;
}

#endif
