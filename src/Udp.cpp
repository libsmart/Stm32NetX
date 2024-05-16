/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Udp.hpp"
#include "Stm32NetX.hpp"
#include "IpInstance.hpp"

using namespace Stm32NetX;

Udp::Udp(NetX &NX, IpInstance &ipInstance) : NX(NX), ipInstance(ipInstance) {
    setLogger(NX.getLogger());
}

UINT Udp::enable() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::Udp::enable()");

    UINT ret = NX_SUCCESS;


    // Enable the UDP protocol
    ret = nx_udp_enable(&ipInstance);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("ICMP enable failed. nx_icmp_enable() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }


    return TX_SUCCESS;
}
