/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <libsmart_config.hpp>
#ifdef LIBSMART_STM32NETX_ENABLE_DHCP
#include "Dhcp.hpp"
#include "Stm32NetX.hpp"
#include "IpInstance.hpp"

using namespace Stm32NetX;

Dhcp::Dhcp(NetX &NX, IpInstance &ipInstance) : NX(NX), ipInstance(ipInstance) {
    setLogger(NX.getLogger());
}

UINT Dhcp::create() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::Dhcp::create()");

    static CHAR dhcpClient_name[] = "Stm32NetX::Dhcp";

    // create the DHCP client
    const auto ret = nx_dhcp_create(this, &ipInstance, dhcpClient_name);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("DHCP create failed. nx_icmp_enable() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }

    return ret;
}

UINT Dhcp::start() {
    const auto ret = nx_dhcp_start(this);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("DHCP start failed. nx_dhcp_start() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT Dhcp::stop() {
    const auto ret = nx_dhcp_stop(this);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("DHCP stop failed. nx_dhcp_stop() = 0x%02x\r\n", ret);
    }
    return ret;
}

UINT Dhcp::reinitialize() {
    const auto ret = nx_dhcp_reinitialize(this);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("DHCP reinitialize failed. nx_dhcp_reinitialize() = 0x%02x\r\n", ret);
    }
    return ret;
}

#endif
