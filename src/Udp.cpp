/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "nx_api.h"
#ifdef NX_IP_UDP_ENABLED

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

    // Enable the UDP protocol
    const auto ret = nx_udp_enable(&ipInstance);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("UDP enable failed. nx_udp_enable() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }

    return ret;
}

#endif