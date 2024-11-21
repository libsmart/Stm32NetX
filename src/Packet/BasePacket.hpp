/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "Loggable.hpp"

namespace Stm32NetX {
    class BasePacket : public Stm32ItmLogger::Loggable {
    public:
        BasePacket() : BasePacket(nullptr, &Stm32ItmLogger::emptyLogger) { ; }

        explicit BasePacket(NX_PACKET *nx_packet) : BasePacket(nx_packet, &Stm32ItmLogger::emptyLogger) { ; }

        BasePacket(NX_PACKET *nx_packet, Stm32ItmLogger::LoggerInterface *logger)
            : Loggable(logger), nxPacket(nx_packet) { ; }


        /**
         * @brief Obtains the packet pool associated with the current packet.
         *
         * This function returns the NX_PACKET_POOL instance that owns the current NX_PACKET.
         * If there is no associated NX_PACKET, it returns a null pointer.
         *
         * @return The NX_PACKET_POOL instance or nullptr if no NX_PACKET is associated.
         */
        NX_PACKET_POOL *getPacketPool() { return nxPacket == nullptr ? nullptr : nxPacket->nx_packet_pool_owner; }


        /**
         * @brief Retrieves the current NX_PACKET instance.
         *
         * This function returns the NX_PACKET instance associated with this object.
         *
         * @return The NX_PACKET instance.
         */
        NX_PACKET *getNxPacket() { return nxPacket; }


        /**
         * @brief Allocates a packet from the specified packet pool.
         *
         * This function calls the NetX function to allocate a packet from the given packet pool and logs the operation.
         *
         * @param pool_ptr Pointer to the packet pool from which the packet is to be allocated.
         * @param packet_type Type of the packet to allocate.
         * @param wait_option Specifies how the thread waits for available packets.
         * @return A status code indicating the success or failure of the allocation operation.
         */
        UINT allocate(NX_PACKET_POOL *pool_ptr, ULONG packet_type, ULONG wait_option);

        /**
         * @brief Copies an existing packet to a new packet.
         *
         * This function calls the NetX function to copy the content of an existing packet to a newly allocated packet
         * from the specified packet pool, and logs the operation.
         *
         * @param new_packet_ptr Pointer to a pointer that will hold the address of the new packet.
         * @param pool_ptr Pointer to the packet pool from which the new packet is to be allocated.
         * @param wait_option Specifies how the thread waits for an available packet.
         * @return A status code indicating the success or failure of the copy operation.
         */
        UINT copy(NX_PACKET **new_packet_ptr, NX_PACKET_POOL *pool_ptr, ULONG wait_option);

        /**
         * @brief Appends data to a packet from the specified data source.
         *
         * This function appends data to the packet starting at the given data pointer, with the specified data size,
         * using a packet from the given packet pool, and logs the operation.
         *
         * @param data_start Pointer to the start of the data to be appended.
         * @param data_size Size of the data to be appended.
         * @param pool_ptr Pointer to the packet pool from which the packet is to be allocated.
         * @param wait_option Specifies how the thread waits for available packets.
         * @return A status code indicating the success or failure of the append operation.
         */
        UINT data_append(VOID *data_start, ULONG data_size, NX_PACKET_POOL *pool_ptr, ULONG wait_option);

        /**
         * @brief Extracts data from the packet at the specified offset.
         *
         * This function extracts data starting from the given offset within the packet and copies it to the provided buffer,
         * logging the operation in the process.
         *
         * @param offset Offset within the packet from where the data should be extracted.
         * @param buffer_start Pointer to the buffer where the extracted data will be copied.
         * @param buffer_length Length of the buffer to which the data will be copied.
         * @param bytes_copied Pointer to a variable where the number of bytes copied will be stored.
         * @return A status code indicating the success or failure of the data extraction operation.
         */
        UINT data_extract_offset(ULONG offset, VOID *buffer_start, ULONG buffer_length, ULONG *bytes_copied);

        /**
         * @brief Retrieves data from the packet into the specified buffer.
         *
         * This function wraps the NetX function to retrieve data from the packet
         * and logs the operation. If the retrieval is unsuccessful, it logs an
         * error message and may throw an exception in debug builds.
         *
         * @param buffer_start Pointer to the start of the buffer where the packet data will be copied.
         * @param bytes_copied Pointer to the variable that will store the number of bytes copied.
         * @return A status code indicating the success or failure of the retrieval operation.
         */
        UINT data_retrieve(VOID *buffer_start, ULONG *bytes_copied);

        /**
         * @brief Retrieves the length of the packet.
         *
         * This function calls the NetX function to obtain the length of the packet and logs the operation.
         *
         * @param length Pointer to a variable where the packet length will be stored.
         * @return A status code indicating the success or failure of the operation.
         */
        UINT length_get(ULONG *length);

        /**
         * @brief Releases a packet back to the NetX packet pool.
         *
         * This function logs the release operation, calls the NetX function to release the specified packet, and
         * handles any errors that occur during the process.
         *
         * @return A status code indicating the success or failure of the release operation.
         */
        UINT release();

        /**
         * @brief Releases a previously transmitted packet.
         *
         * This function logs the attempt to release a transmitted packet and invokes the NetX function to release the packet.
         * If the release operation fails, it logs the error and, if exceptions are enabled, throws a runtime error.
         *
         * @return A status code indicating the success or failure of the release operation.
         */
        UINT transmit_release();

    protected:
        /** Stores a pointer to the NX_PACKET structure */
        NX_PACKET *nxPacket;
    };
}
