/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <stdexcept>

#include "BasePacket.hpp"
#include "nx_api.h"
#include "PacketPool.hpp"

namespace Stm32NetX {
    class Packet : public BasePacket {
    public:
        Packet() = default;

        explicit Packet(NX_PACKET *nx_packet)
            : BasePacket(nx_packet) { ; }

        Packet(NX_PACKET *nx_packet, Stm32ItmLogger::LoggerInterface *logger)
            : BasePacket(nx_packet, logger) { ; }

        explicit operator NX_PACKET *() { return getNxPacket(); }

        Packet &operator=(NX_PACKET *nxPacket) {
            if (this->nxPacket != nullptr) {
#if __EXCEPTIONS
                throw std::runtime_error("Packet already contains a NX_PACKET");
#endif
                return *this;
            }
            this->nxPacket = nxPacket;
            return *this;
        }


        void forget() { nxPacket = nullptr; }


        /**
         * Appends data to an existing packet.
         *
         * @param data_start Pointer to the start of the data to be appended.
         * @param data_size Size of the data to be appended in bytes.
         * @return UINT Status of the data append operation, with NX_SUCCESS indicating success and other values indicating failure.
         */
        UINT dataAppend(VOID *data_start, ULONG data_size);


        /**
         * Retrieves the length of the packet.
         *
         * @return ULONG The length of the packet in bytes.
         */
        ULONG lengthGet();

    protected:
        ULONG waitOption = NX_WAIT_FOREVER;
    };
}
