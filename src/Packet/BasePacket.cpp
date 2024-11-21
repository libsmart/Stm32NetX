/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "BasePacket.hpp"
#include <stdexcept>

using namespace Stm32NetX;

UINT BasePacket::allocate(NX_PACKET_POOL *pool_ptr, ULONG packet_type, ULONG wait_option) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::BasePacket::allocate()");

    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_packet_allocate
    const auto ret = nx_packet_allocate(pool_ptr, &nxPacket, packet_type, wait_option);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("nx_packet_allocate() = 0x%02x\r\n", ret);
#if __EXCEPTIONS
        throw std::runtime_error("nx_packet_allocate() failed");
#endif
        return ret;
    }
    return ret;
}

UINT BasePacket::copy(NX_PACKET **new_packet_ptr, NX_PACKET_POOL *pool_ptr, ULONG wait_option) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::BasePacket::copy()");

    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_packet_copy
    const auto ret = nx_packet_copy(nxPacket, new_packet_ptr, pool_ptr, wait_option);
    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("nx_packet_copy() = 0x%02x\r\n", ret);
#if __EXCEPTIONS
        throw std::runtime_error("nx_packet_copy() failed");
#endif
        return ret;
    }
    return ret;
}

UINT BasePacket::data_append(void *data_start, ULONG data_size, NX_PACKET_POOL *pool_ptr, ULONG wait_option) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::BasePacket::data_append()");

    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_packet_data_append
    const auto ret = nx_packet_data_append(nxPacket, data_start, data_size, pool_ptr, wait_option);

    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("nx_packet_data_append() = 0x%02x\r\n", ret);
#if __EXCEPTIONS
        throw std::runtime_error("nx_packet_data_append() failed");
#endif
        return ret;
    }
    return ret;
}

UINT BasePacket::data_extract_offset(ULONG offset, void *buffer_start, ULONG buffer_length, ULONG *bytes_copied) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::BasePacket::data_extract_offset()");

    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_packet_data_extract_offset
    const auto ret = nx_packet_data_extract_offset(nxPacket, offset, buffer_start, buffer_length, bytes_copied);

    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("nx_packet_data_extract_offset() = 0x%02x\r\n", ret);
#if __EXCEPTIONS
        throw std::runtime_error("nx_packet_data_extract_offset() failed");
#endif
        return ret;
    }
    return ret;
}

UINT BasePacket::data_retrieve(void *buffer_start, ULONG *bytes_copied) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::BasePacket::data_retrieve()");

    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_packet_data_retrieve
    const auto ret = nx_packet_data_retrieve(nxPacket, buffer_start, bytes_copied);

    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("nx_packet_data_retrieve() = 0x%02x\r\n", ret);
#if __EXCEPTIONS
        throw std::runtime_error("nx_packet_data_retrieve() failed");
#endif
        return ret;
    }
    return ret;
}

UINT BasePacket::length_get(ULONG *length) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::BasePacket::length_get()");

    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_packet_length_get
    const auto ret = nx_packet_length_get(nxPacket, length);

    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("nx_packet_length_get() = 0x%02x\r\n", ret);
#if __EXCEPTIONS
        throw std::runtime_error("nx_packet_length_get() failed");
#endif
        return ret;
    }
    return ret;
}

UINT BasePacket::release() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::BasePacket::release()");

    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_packet_release
    const auto ret = nx_packet_release(nxPacket);

    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("nx_packet_release() = 0x%02x\r\n", ret);
#if __EXCEPTIONS
        throw std::runtime_error("nx_packet_release() failed");
#endif
        return ret;
    }
    nxPacket = nullptr;
    return ret;
}

UINT BasePacket::transmit_release() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32NetX::BasePacket::transmit_release()");

    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter4.md#nx_packet_transmit_release
    const auto ret = nx_packet_transmit_release(nxPacket);

    if (ret != NX_SUCCESS) {
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("nx_packet_transmit_release() = 0x%02x\r\n", ret);
#if __EXCEPTIONS
        throw std::runtime_error("nx_packet_transmit_release() failed");
#endif
        return ret;
    }
    nxPacket = nullptr;
    return ret;
}
