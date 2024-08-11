/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "PacketPool.hpp"
#include "Stm32NetX.hpp"
using namespace Stm32NetX;

PacketPool::PacketPool(NetX &NX) : NX_PACKET_POOL(), NX(NX) {
    setLogger(NX.getLogger());
}

// UINT PacketPool::create() {
    // return create(LIBSMART_STM32NETX_PAYLOAD_SIZE, memPacketpool);
// }

UINT PacketPool::create(ULONG payload_size, ULONG *packetPoolMemory, size_t packetPoolMemorySize) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::PacketPool::create()");

    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_packet_pool_create
    const auto ret = nx_packet_pool_create(this,
                                           getNameNonConst(),
                                           payload_size,
                                           packetPoolMemory,
                                           packetPoolMemorySize);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Packet pool creation failed. nx_packet_pool_create() = 0x%02x\r\n", ret);
        assert_param(ret == NX_SUCCESS);
        return NX_NOT_ENABLED;
    }

    return ret;
}
