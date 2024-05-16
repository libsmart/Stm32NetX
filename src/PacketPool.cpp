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

UINT PacketPool::create() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::PacketPool::create()");

    UINT ret = NX_SUCCESS;
    UCHAR *memPtr = nullptr;
    CHAR packetPool_name[] = "Stm32NetX::PacketPool";

    // Allocate the memory for packet_pool
    /*
    ret = tx_byte_allocate(NX.byte_pool, reinterpret_cast<void **>(&memPtr),
                           LIBSMART_STM32NETX_PACKET_POOL_SIZE, TX_NO_WAIT);
    if (ret != TX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Packet pool allocation failed. tx_byte_allocate() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }
    */


    // Create the Packet pool to be used for packet allocation
    ret = nx_packet_pool_create(this,
                                packetPool_name,
                                LIBSMART_STM32NETX_PAYLOAD_SIZE,
                                NX.bytePool.allocate(LIBSMART_STM32NETX_PACKET_POOL_SIZE),
                                LIBSMART_STM32NETX_PACKET_POOL_SIZE);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("Packet pool creation failed. nx_packet_pool_create() = 0x%02x\r\n", ret);
        return NX_NOT_ENABLED;
    }


    return TX_SUCCESS;
}
