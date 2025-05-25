/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Packet.hpp"
#include <stdexcept>

using namespace Stm32NetX;

UINT Packet::dataAppend(const uint8_t *data_start, const ULONG data_size) {
    return BasePacket::data_append((void *)(data_start), data_size, getPacketPool(), waitOption);
}

ULONG Packet::lengthGet() {
    ULONG length = 0;
    BasePacket::length_get(&length);
    return length;
}

