/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <stdexcept>

#include "BasePacket.hpp"
#include "nx_api.h"
#include "PacketPool.hpp"
#include "String/FixedString.hpp"

namespace Stm32NetX {
    class Packet : public BasePacket, public Stm32Common::Print {
    public:
        Packet() = default;

        explicit Packet(NX_PACKET *nx_packet) : BasePacket(nx_packet) { ; }

        explicit Packet(NX_PACKET &nx_packet) : BasePacket(&nx_packet) { ; }

        Packet(NX_PACKET *nx_packet, Stm32ItmLogger::LoggerInterface *logger)
            : BasePacket(nx_packet, logger) { ; }

        Packet(NX_PACKET &nx_packet, Stm32ItmLogger::LoggerInterface *logger)
            : BasePacket(&nx_packet, logger) { ; }

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


        /**
         * Resets the internal NX_PACKET pointer to null, effectively "forgetting" the current packet.
         * @note This function does not release the packet
         */
        void forget() { nxPacket = nullptr; }


        /**
         * Appends data to an existing packet.
         *
         * @param data_start Pointer to the start of the data to be appended.
         * @param data_size Size of the data to be appended in bytes.
         * @return UINT Status of the data append operation, with NX_SUCCESS indicating success and other values indicating failure.
         */
        UINT dataAppend(const uint8_t *data_start, ULONG data_size);


        /**
         * Appends a null-terminated character array to an existing packet.
         *
         * @tparam N The size of the character array, including the null terminator.
         * @param data A reference to the character array to be appended.
         * @return UINT Status of the append operation, with NX_SUCCESS indicating success and other values indicating failure.
         */
        template<size_t N>
        UINT dataAppend(const char (&data)[N] ) {
            return dataAppend(static_cast<const uint8_t *>(data), N  - 1);
        }

        /**
         * Appends a fixed-size array of data to the packet.
         *
         * @tparam N The size of the array to be appended.
         * @param data A reference to the fixed-size array containing the data to append.
         * @return UINT The result of the append operation, with NX_SUCCESS indicating success and other values indicating failure.
         */
        template<size_t N>
        UINT dataAppend(const uint8_t (&data)[N] ) {
            return dataAppend(data, N);
        }

        /**
         * Appends the contents of a fixed-size string to the packet.
         *
         * @tparam N The maximum size of the fixed-size string, including the null terminator.
         * @param str A reference to the fixed-size string to be appended.
         * @return UINT The result of the append operation, with NX_SUCCESS indicating success, and other values indicating failure.
         */
        template<size_t N>
        UINT dataAppend(const Stm32Common::String::FixedString<N> &str) {
            return dataAppend(reinterpret_cast<const uint8_t *>(str.c_str()), str.size());
        }

        /**
         * Retrieves the length of the packet.
         *
         * @return ULONG The length of the packet in bytes.
         */
        ULONG lengthGet();


        /**
         * Retrieves a write buffer for the packet.
         * Always returns nullptr, since NetX's packet services do not support direct buffer write.
         *
         * @param[out] buffer A reference to a pointer that will be set to the write buffer.
         * @return size_t The size of the buffer, which in this implementation is always 0.
         */
        size_t getWriteBuffer(uint8_t *&buffer) override {
            buffer = nullptr;
            return 0;
        }

        /**
         * Sets the number of bytes written to the packet.
         * Not supported. @see Packet::getWriteBuffer(uint8_t *&buffer)
         *
         * @param size The number of bytes to be set as written.
         * @return size_t The actual number of bytes written, which in this implementation is always 0.
         */
        size_t setWrittenBytes(size_t size) override {
            return 0;
        }

        /**
         * Writes a single byte of data to the current packet.
         *
         * @param data The byte to be written.
         * @return size_t Returns 1 if the write operation was successful; otherwise, returns 0.
         */
        size_t write(uint8_t data) override {
            return dataAppend(&data, 1) == NX_SUCCESS ? 1 : 0;
        }

        /**
         * Writes an array of bytes to the current packet.
         *
         * @param inputBytes Pointer to the array of bytes to be written.
         * @param size The number of bytes to write.
         * @return size_t The number of bytes successfully written, or 0 if the operation failed.
         */
        size_t write(const uint8_t *inputBytes, size_t size) override {
            return dataAppend(const_cast<uint8_t *>(inputBytes), size) == NX_SUCCESS ? size : 0;
        }

        /**
         * Determines the number of bytes available for writing in the packet.
         *
         * @return int The number of bytes available for write, calculated as the difference between the packet's total
         * length and the length that has already been used.
         */
        int availableForWrite() override {
            return (nxPacket->nx_packet_data_end - nxPacket->nx_packet_data_start) - lengthGet();
        }

        /**
         * This method overrides the base class's flush method.
         * It currently performs no operations.
         */
        void flush() override { ; }

    protected:
        ULONG waitOption = NX_WAIT_FOREVER;
    };
}
