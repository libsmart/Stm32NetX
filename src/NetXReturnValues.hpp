/*
 * SPDX-FileCopyrightText: 2025 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#pragma once
#include <libsmart_config.hpp>
#include <climits>
#include <optional>
#include <variant>
#include "Hash/Fnv1a.hpp"
#include "I18N/gettext_map.hpp"

extern "C" {
#include "nx_api.h"
}

using namespace Stm32Common::Hash;

#if defined(LIBSMART_ENABLE_ERROR_STRINGS)
#define LIBSMART_CONSTEXPR_CHAR(name, str) static constexpr char name[] = str;
#else
#define LIBSMART_CONSTEXPR_CHAR(name, str)
#endif


namespace Stm32NetX {
    /**
     * @struct NetXReturnValues
     * @brief Represents a collection of structures containing constant hash values and API return codes for various NetX return statuses.
     *
     * The NetXReturnValues struct defines a series of nested structs, each representing a unique return status
     * with an associated static hash value and API return code. These values are used to identify and handle
     * specific outcomes or errors in a NetX-related context.
     *
     * Each nested struct contains:
     *   - `strHash`: A compile-time hash value generated from the corresponding status string using the FNV-1a algorithm.
     *   - `apiReturnValue`: The numeric return value associated with the status.
     */
    struct NetXReturnValues {
        struct NONE {
            LIBSMART_CONSTEXPR_CHAR(name, "NONE");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NONE");
            static constexpr UINT apiReturnValue = UINT_MAX;
        };

        struct SUCCESS {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_SUCCESS");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_SUCCESS");
            static constexpr UINT apiReturnValue = NX_SUCCESS;
        };

        struct NO_PACKET {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NO_PACKET");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NO_PACKET");
            static constexpr UINT apiReturnValue = NX_NO_PACKET;
        };

        struct UNDERFLOW {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_UNDERFLOW");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_UNDERFLOW");
            static constexpr UINT apiReturnValue = NX_UNDERFLOW;
        };

        struct OVERFLOW {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_OVERFLOW");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_OVERFLOW");
            static constexpr UINT apiReturnValue = NX_OVERFLOW;
        };

        struct NO_MAPPING {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NO_MAPPING");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NO_MAPPING");
            static constexpr UINT apiReturnValue = NX_NO_MAPPING;
        };

        struct DELETED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_DELETED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_DELETED");
            static constexpr UINT apiReturnValue = NX_DELETED;
        };

        struct POOL_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_POOL_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_POOL_ERROR");
            static constexpr UINT apiReturnValue = NX_POOL_ERROR;
        };

        struct PTR_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_PTR_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_PTR_ERROR");
            static constexpr UINT apiReturnValue = NX_PTR_ERROR;
        };

        struct WAIT_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_WAIT_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_WAIT_ERROR");
            static constexpr UINT apiReturnValue = NX_WAIT_ERROR;
        };

        struct SIZE_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_SIZE_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_SIZE_ERROR");
            static constexpr UINT apiReturnValue = NX_SIZE_ERROR;
        };

        struct OPTION_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_OPTION_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_OPTION_ERROR");
            static constexpr UINT apiReturnValue = NX_OPTION_ERROR;
        };

        struct DELETE_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_DELETE_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_DELETE_ERROR");
            static constexpr UINT apiReturnValue = NX_DELETE_ERROR;
        };

        struct CALLER_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_CALLER_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_CALLER_ERROR");
            static constexpr UINT apiReturnValue = NX_CALLER_ERROR;
        };

        struct INVALID_PACKET {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_INVALID_PACKET");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_INVALID_PACKET");
            static constexpr UINT apiReturnValue = NX_INVALID_PACKET;
        };

        struct INVALID_SOCKET {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_INVALID_SOCKET");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_INVALID_SOCKET");
            static constexpr UINT apiReturnValue = NX_INVALID_SOCKET;
        };

        struct NOT_ENABLED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_ENABLED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_ENABLED");
            static constexpr UINT apiReturnValue = NX_NOT_ENABLED;
        };

        struct ALREADY_ENABLED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_ALREADY_ENABLED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_ALREADY_ENABLED");
            static constexpr UINT apiReturnValue = NX_ALREADY_ENABLED;
        };

        struct ENTRY_NOT_FOUND {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_ENTRY_NOT_FOUND");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_ENTRY_NOT_FOUND");
            static constexpr UINT apiReturnValue = NX_ENTRY_NOT_FOUND;
        };

        struct NO_MORE_ENTRIES {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NO_MORE_ENTRIES");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NO_MORE_ENTRIES");
            static constexpr UINT apiReturnValue = NX_NO_MORE_ENTRIES;
        };

        struct ARP_TIMER_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_ARP_TIMER_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_ARP_TIMER_ERROR");
            static constexpr UINT apiReturnValue = NX_ARP_TIMER_ERROR;
        };

        struct RESERVED_CODE0 {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_RESERVED_CODE0");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_RESERVED_CODE0");
            static constexpr UINT apiReturnValue = NX_RESERVED_CODE0;
        };

        struct WAIT_ABORTED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_WAIT_ABORTED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_WAIT_ABORTED");
            static constexpr UINT apiReturnValue = NX_WAIT_ABORTED;
        };

        struct IP_INTERNAL_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_IP_INTERNAL_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_IP_INTERNAL_ERROR");
            static constexpr UINT apiReturnValue = NX_IP_INTERNAL_ERROR;
        };

        struct IP_ADDRESS_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_IP_ADDRESS_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_IP_ADDRESS_ERROR");
            static constexpr UINT apiReturnValue = NX_IP_ADDRESS_ERROR;
        };

        struct ALREADY_BOUND {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_ALREADY_BOUND");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_ALREADY_BOUND");
            static constexpr UINT apiReturnValue = NX_ALREADY_BOUND;
        };

        struct PORT_UNAVAILABLE {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_PORT_UNAVAILABLE");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_PORT_UNAVAILABLE");
            static constexpr UINT apiReturnValue = NX_PORT_UNAVAILABLE;
        };

        struct NOT_BOUND {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_BOUND");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_BOUND");
            static constexpr UINT apiReturnValue = NX_NOT_BOUND;
        };

        struct RESERVED_CODE1 {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_RESERVED_CODE1");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_RESERVED_CODE1");
            static constexpr UINT apiReturnValue = NX_RESERVED_CODE1;
        };

        struct SOCKET_UNBOUND {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_SOCKET_UNBOUND");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_SOCKET_UNBOUND");
            static constexpr UINT apiReturnValue = NX_SOCKET_UNBOUND;
        };

        struct NOT_CREATED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_CREATED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_CREATED");
            static constexpr UINT apiReturnValue = NX_NOT_CREATED;
        };

        struct SOCKETS_BOUND {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_SOCKETS_BOUND");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_SOCKETS_BOUND");
            static constexpr UINT apiReturnValue = NX_SOCKETS_BOUND;
        };

        struct NO_RESPONSE {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NO_RESPONSE");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NO_RESPONSE");
            static constexpr UINT apiReturnValue = NX_NO_RESPONSE;
        };

        struct POOL_DELETED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_POOL_DELETED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_POOL_DELETED");
            static constexpr UINT apiReturnValue = NX_POOL_DELETED;
        };

        struct ALREADY_RELEASED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_ALREADY_RELEASED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_ALREADY_RELEASED");
            static constexpr UINT apiReturnValue = NX_ALREADY_RELEASED;
        };

        struct RESERVED_CODE2 {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_RESERVED_CODE2");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_RESERVED_CODE2");
            static constexpr UINT apiReturnValue = NX_RESERVED_CODE2;
        };

        struct MAX_LISTEN {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_MAX_LISTEN");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_MAX_LISTEN");
            static constexpr UINT apiReturnValue = NX_MAX_LISTEN;
        };

        struct DUPLICATE_LISTEN {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_DUPLICATE_LISTEN");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_DUPLICATE_LISTEN");
            static constexpr UINT apiReturnValue = NX_DUPLICATE_LISTEN;
        };

        struct NOT_CLOSED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_CLOSED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_CLOSED");
            static constexpr UINT apiReturnValue = NX_NOT_CLOSED;
        };

        struct NOT_LISTEN_STATE {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_LISTEN_STATE");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_LISTEN_STATE");
            static constexpr UINT apiReturnValue = NX_NOT_LISTEN_STATE;
        };

        struct IN_PROGRESS {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_IN_PROGRESS");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_IN_PROGRESS");
            static constexpr UINT apiReturnValue = NX_IN_PROGRESS;
        };

        struct NOT_CONNECTED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_CONNECTED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_CONNECTED");
            static constexpr UINT apiReturnValue = NX_NOT_CONNECTED;
        };

        struct WINDOW_OVERFLOW {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_WINDOW_OVERFLOW");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_WINDOW_OVERFLOW");
            static constexpr UINT apiReturnValue = NX_WINDOW_OVERFLOW;
        };

        struct ALREADY_SUSPENDED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_ALREADY_SUSPENDED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_ALREADY_SUSPENDED");
            static constexpr UINT apiReturnValue = NX_ALREADY_SUSPENDED;
        };

        struct DISCONNECT_FAILED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_DISCONNECT_FAILED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_DISCONNECT_FAILED");
            static constexpr UINT apiReturnValue = NX_DISCONNECT_FAILED;
        };

        struct STILL_BOUND {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_STILL_BOUND");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_STILL_BOUND");
            static constexpr UINT apiReturnValue = NX_STILL_BOUND;
        };

        struct NOT_SUCCESSFUL {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_SUCCESSFUL");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_SUCCESSFUL");
            static constexpr UINT apiReturnValue = NX_NOT_SUCCESSFUL;
        };

        struct UNHANDLED_COMMAND {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_UNHANDLED_COMMAND");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_UNHANDLED_COMMAND");
            static constexpr UINT apiReturnValue = NX_UNHANDLED_COMMAND;
        };

        struct NO_FREE_PORTS {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NO_FREE_PORTS");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NO_FREE_PORTS");
            static constexpr UINT apiReturnValue = NX_NO_FREE_PORTS;
        };

        struct INVALID_PORT {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_INVALID_PORT");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_INVALID_PORT");
            static constexpr UINT apiReturnValue = NX_INVALID_PORT;
        };

        struct INVALID_RELISTEN {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_INVALID_RELISTEN");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_INVALID_RELISTEN");
            static constexpr UINT apiReturnValue = NX_INVALID_RELISTEN;
        };

        struct CONNECTION_PENDING {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_CONNECTION_PENDING");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_CONNECTION_PENDING");
            static constexpr UINT apiReturnValue = NX_CONNECTION_PENDING;
        };

        struct TX_QUEUE_DEPTH {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_TX_QUEUE_DEPTH");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_TX_QUEUE_DEPTH");
            static constexpr UINT apiReturnValue = NX_TX_QUEUE_DEPTH;
        };

        struct NOT_IMPLEMENTED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_IMPLEMENTED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_IMPLEMENTED");
            static constexpr UINT apiReturnValue = NX_NOT_IMPLEMENTED;
        };

        struct NOT_SUPPORTED {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_SUPPORTED");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_SUPPORTED");
            static constexpr UINT apiReturnValue = NX_NOT_SUPPORTED;
        };

        struct INVALID_INTERFACE {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_INVALID_INTERFACE");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_INVALID_INTERFACE");
            static constexpr UINT apiReturnValue = NX_INVALID_INTERFACE;
        };

        struct INVALID_PARAMETERS {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_INVALID_PARAMETERS");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_INVALID_PARAMETERS");
            static constexpr UINT apiReturnValue = NX_INVALID_PARAMETERS;
        };

        struct NOT_FOUND {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NOT_FOUND");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NOT_FOUND");
            static constexpr UINT apiReturnValue = NX_NOT_FOUND;
        };

        struct CANNOT_START {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_CANNOT_START");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_CANNOT_START");
            static constexpr UINT apiReturnValue = NX_CANNOT_START;
        };

        struct NO_INTERFACE_ADDRESS {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_NO_INTERFACE_ADDRESS");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_NO_INTERFACE_ADDRESS");
            static constexpr UINT apiReturnValue = NX_NO_INTERFACE_ADDRESS;
        };

        struct INVALID_MTU_DATA {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_INVALID_MTU_DATA");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_INVALID_MTU_DATA");
            static constexpr UINT apiReturnValue = NX_INVALID_MTU_DATA;
        };

        struct DUPLICATED_ENTRY {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_DUPLICATED_ENTRY");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_DUPLICATED_ENTRY");
            static constexpr UINT apiReturnValue = NX_DUPLICATED_ENTRY;
        };

        struct PACKET_OFFSET_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_PACKET_OFFSET_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_PACKET_OFFSET_ERROR");
            static constexpr UINT apiReturnValue = NX_PACKET_OFFSET_ERROR;
        };

        struct OPTION_HEADER_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_OPTION_HEADER_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_OPTION_HEADER_ERROR");
            static constexpr UINT apiReturnValue = NX_OPTION_HEADER_ERROR;
        };

        struct CONTINUE {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_CONTINUE");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_CONTINUE");
            static constexpr UINT apiReturnValue = NX_CONTINUE;
        };

        struct TCPIP_OFFLOAD_ERROR {
            LIBSMART_CONSTEXPR_CHAR(name, "NX_TCPIP_OFFLOAD_ERROR");
            static constexpr FNV1a::fnv1a32_t strHash = FNV1a::hash("NX_TCPIP_OFFLOAD_ERROR");
            static constexpr UINT apiReturnValue = NX_TCPIP_OFFLOAD_ERROR;
        };

        using Variant = std::variant<
            NONE,
            SUCCESS,
            NO_PACKET,
            UNDERFLOW,
            OVERFLOW,
            NO_MAPPING,
            DELETED,
            POOL_ERROR,
            PTR_ERROR,
            WAIT_ERROR,
            SIZE_ERROR,
            OPTION_ERROR,
            DELETE_ERROR,
            CALLER_ERROR,
            INVALID_PACKET,
            INVALID_SOCKET,
            NOT_ENABLED,
            ALREADY_ENABLED,
            ENTRY_NOT_FOUND,
            NO_MORE_ENTRIES,
            ARP_TIMER_ERROR,
            RESERVED_CODE0,
            WAIT_ABORTED,
            IP_INTERNAL_ERROR,
            IP_ADDRESS_ERROR,
            ALREADY_BOUND,
            PORT_UNAVAILABLE,
            NOT_BOUND,
            RESERVED_CODE1,
            SOCKET_UNBOUND,
            NOT_CREATED,
            SOCKETS_BOUND,
            NO_RESPONSE,
            POOL_DELETED,
            ALREADY_RELEASED,
            RESERVED_CODE2,
            MAX_LISTEN,
            DUPLICATE_LISTEN,
            NOT_CLOSED,
            NOT_LISTEN_STATE,
            IN_PROGRESS,
            NOT_CONNECTED,
            WINDOW_OVERFLOW,
            ALREADY_SUSPENDED,
            DISCONNECT_FAILED,
            STILL_BOUND,
            NOT_SUCCESSFUL,
            UNHANDLED_COMMAND,
            NO_FREE_PORTS,
            INVALID_PORT,
            INVALID_RELISTEN,
            CONNECTION_PENDING,
            TX_QUEUE_DEPTH,
            NOT_IMPLEMENTED,
            NOT_SUPPORTED,
            INVALID_INTERFACE,
            INVALID_PARAMETERS,
            NOT_FOUND,
            CANNOT_START,
            NO_INTERFACE_ADDRESS,
            INVALID_MTU_DATA,
            DUPLICATED_ENTRY,
            PACKET_OFFSET_ERROR,
            OPTION_HEADER_ERROR,
            CONTINUE,
            TCPIP_OFFLOAD_ERROR
        >;


        /**
         * @struct NetXReturnValue
         * @brief Encapsulates a flexible return structure for NetX operations, supporting multiple return types and associated attributes.
         *
         * The NetXReturnValue structure is designed to handle various types of return values and metadata for NetX-related
         * processes. It provides functionality to store, retrieve, and manipulate these values consistently across different scenarios.
         *
         * Key Features:
         * - Supports initialization with different types, including default NONE type, optional `Variant`, raw `Variant`,
         *   `UINT` values for API return codes, and C-string messages.
         * - Provides methods to extract hash values (`strHash`), API return values (`apiReturnValue`), and message strings (`message`).
         * - Allows visitors to access and operate on the stored value via `visit` methods for both mutable and const visitors.
         * - Maintains access to the raw stored `Variant` value through the `raw` method.
         *
         * Members:
         * - `value`: Stores the return value as a flexible `Variant`.
         * - `apiRetVal`: Holds a numeric API return value, defaulting to `UINT_MAX` when unspecified.
         * - `msg`: Contains an optional descriptive message as a C-string.
         */
        struct NetXReturnValue {
            NetXReturnValue() : value(NONE{}) { ; }

            template<typename E>
            explicit NetXReturnValue(const E e) : value(e) { ; }

            explicit NetXReturnValue(const std::optional<Variant> e) : value(e.value_or(NONE{})) { ; }

            NetXReturnValue(const Variant e) : value(e) { ; }
            NetXReturnValue(const UINT e) : value(NONE{}), apiRetVal(e) { ; }
            NetXReturnValue(const char *e) : value(NONE{}), msg(e) { ; }

            FNV1a::fnv1a32_t strHash() const {
                return std::visit([](auto &&e) { return e.strHash; }, value);
            }

            UINT apiReturnValue() const {
                if (std::holds_alternative<NONE>(value)) { return apiRetVal; }
                return std::visit([](auto &&e) { return e.apiReturnValue; }, value);
            }

#if defined(LIBSMART_ENABLE_ERROR_STRINGS)
            const char *apiReturnString() const {
                return std::visit([](auto &&e) { return e.name; }, value);
            }
#endif

            const char *message() const {
                return msg;
            }

            template<typename Visitor>
            decltype(auto) visit(Visitor &&v) {
                return std::visit(std::forward<Visitor>(v), value);
            }

            template<typename Visitor>
            decltype(auto) visit(Visitor &&v) const {
                return std::visit(std::forward<Visitor>(v), value);
            }

            const Variant &raw() const { return value; }

        private:
            Variant value;
            UINT apiRetVal = UINT_MAX;
            const char *msg = "";
        };


        // Eigene Implementierung von 'type_identity' für C++17
        template<typename T>
        struct type_identity {
            using type = T;
        };

        // Hilfsfunktion: Schleife über alle Typen der `Variant`
        template<typename Func, size_t... Is>
        static void forEachTypeImpl(Func &&func, std::index_sequence<Is...>) {
            using VariantType = Variant;
            (..., func(type_identity<std::variant_alternative_t<Is, VariantType> >{}));
        }

        template<typename Func>
        static void forEachType(Func &&func) {
            constexpr size_t VariantSize = std::variant_size<Variant>::value;
            forEachTypeImpl(std::forward<Func>(func), std::make_index_sequence<VariantSize>{});
        }

        static auto find(uint32_t subject) -> std::optional<Variant> {
            std::optional<Variant> result;

            forEachType(
                [&subject, &result](const auto type_info) -> void {
                    using T = typename decltype(type_info)::type;
                    T instance{};
                    if (instance.strHash == subject || instance.apiReturnValue == subject) {
                        result = instance;
                    }
                });

            return result;
        }
    };


    struct NetXApiReturnValues {
        static constexpr std::array<std::pair<UINT, const char *>, 65> errorMappings{
            {
                {NX_SUCCESS, "NX_SUCCESS"},
                {NX_NO_PACKET, "NX_NO_PACKET"},
                {NX_UNDERFLOW, "NX_UNDERFLOW"},
                {NX_OVERFLOW, "NX_OVERFLOW"},
                {NX_NO_MAPPING, "NX_NO_MAPPING"},
                {NX_DELETED, "NX_DELETED"},
                {NX_POOL_ERROR, "NX_POOL_ERROR"},
                {NX_PTR_ERROR, "NX_PTR_ERROR"},
                {NX_WAIT_ERROR, "NX_WAIT_ERROR"},
                {NX_SIZE_ERROR, "NX_SIZE_ERROR"},
                {NX_OPTION_ERROR, "NX_OPTION_ERROR"},
                {NX_DELETE_ERROR, "NX_DELETE_ERROR"},
                {NX_CALLER_ERROR, "NX_CALLER_ERROR"},
                {NX_INVALID_PACKET, "NX_INVALID_PACKET"},
                {NX_INVALID_SOCKET, "NX_INVALID_SOCKET"},
                {NX_NOT_ENABLED, "NX_NOT_ENABLED"},
                {NX_ALREADY_ENABLED, "NX_ALREADY_ENABLED"},
                {NX_ENTRY_NOT_FOUND, "NX_ENTRY_NOT_FOUND"},
                {NX_NO_MORE_ENTRIES, "NX_NO_MORE_ENTRIES"},
                {NX_ARP_TIMER_ERROR, "NX_ARP_TIMER_ERROR"},
                {NX_RESERVED_CODE0, "NX_RESERVED_CODE0"},
                {NX_WAIT_ABORTED, "NX_WAIT_ABORTED"},
                {NX_IP_INTERNAL_ERROR, "NX_IP_INTERNAL_ERROR"},
                {NX_IP_ADDRESS_ERROR, "NX_IP_ADDRESS_ERROR"},
                {NX_ALREADY_BOUND, "NX_ALREADY_BOUND"},
                {NX_PORT_UNAVAILABLE, "NX_PORT_UNAVAILABLE"},
                {NX_NOT_BOUND, "NX_NOT_BOUND"},
                {NX_RESERVED_CODE1, "NX_RESERVED_CODE1"},
                {NX_SOCKET_UNBOUND, "NX_SOCKET_UNBOUND"},
                {NX_NOT_CREATED, "NX_NOT_CREATED"},
                {NX_SOCKETS_BOUND, "NX_SOCKETS_BOUND"},
                {NX_NO_RESPONSE, "NX_NO_RESPONSE"},
                {NX_POOL_DELETED, "NX_POOL_DELETED"},
                {NX_ALREADY_RELEASED, "NX_ALREADY_RELEASED"},
                {NX_RESERVED_CODE2, "NX_RESERVED_CODE2"},
                {NX_MAX_LISTEN, "NX_MAX_LISTEN"},
                {NX_DUPLICATE_LISTEN, "NX_DUPLICATE_LISTEN"},
                {NX_NOT_CLOSED, "NX_NOT_CLOSED"},
                {NX_NOT_LISTEN_STATE, "NX_NOT_LISTEN_STATE"},
                {NX_IN_PROGRESS, "NX_IN_PROGRESS"},
                {NX_NOT_CONNECTED, "NX_NOT_CONNECTED"},
                {NX_WINDOW_OVERFLOW, "NX_WINDOW_OVERFLOW"},
                {NX_ALREADY_SUSPENDED, "NX_ALREADY_SUSPENDED"},
                {NX_DISCONNECT_FAILED, "NX_DISCONNECT_FAILED"},
                {NX_STILL_BOUND, "NX_STILL_BOUND"},
                {NX_NOT_SUCCESSFUL, "NX_NOT_SUCCESSFUL"},
                {NX_UNHANDLED_COMMAND, "NX_UNHANDLED_COMMAND"},
                {NX_NO_FREE_PORTS, "NX_NO_FREE_PORTS"},
                {NX_INVALID_PORT, "NX_INVALID_PORT"},
                {NX_INVALID_RELISTEN, "NX_INVALID_RELISTEN"},
                {NX_CONNECTION_PENDING, "NX_CONNECTION_PENDING"},
                {NX_TX_QUEUE_DEPTH, "NX_TX_QUEUE_DEPTH"},
                {NX_NOT_IMPLEMENTED, "NX_NOT_IMPLEMENTED"},
                {NX_NOT_SUPPORTED, "NX_NOT_SUPPORTED"},
                {NX_INVALID_INTERFACE, "NX_INVALID_INTERFACE"},
                {NX_INVALID_PARAMETERS, "NX_INVALID_PARAMETERS"},
                {NX_NOT_FOUND, "NX_NOT_FOUND"},
                {NX_CANNOT_START, "NX_CANNOT_START"},
                {NX_NO_INTERFACE_ADDRESS, "NX_NO_INTERFACE_ADDRESS"},
                {NX_INVALID_MTU_DATA, "NX_INVALID_MTU_DATA"},
                {NX_DUPLICATED_ENTRY, "NX_DUPLICATED_ENTRY"},
                {NX_PACKET_OFFSET_ERROR, "NX_PACKET_OFFSET_ERROR"},
                {NX_OPTION_HEADER_ERROR, "NX_OPTION_HEADER_ERROR"},
                {NX_CONTINUE, "NX_CONTINUE"},
                {NX_TCPIP_OFFLOAD_ERROR, "NX_TCPIP_OFFLOAD_ERROR}"}
            }
        };

        static const char *getErrorString(UINT errorCode) {
            const auto it = std::find_if(
                errorMappings.begin(),
                errorMappings.end(),
                [code = errorCode](const std::pair<UINT, const char *> &element) {
                    return element.first == code;
                }
            );
            return (it != errorMappings.end()) ? it->second : AppCore::I18N::UNKNOWN_ERROR_STRING;
        };

        static constexpr std::array<std::pair<UINT, const char *>, 1> errorMappingsPretty{
                {
                    {NX_SUCCESS, "NX_SUCCESS"},
                    //{NX_NOT_CONNECTED, "Not connected"}
                }
        };

        static const char *getPrettyErrorString(UINT errorCode) {
            const auto it = std::find_if(
                errorMappingsPretty.begin(),
                errorMappingsPretty.end(),
                [code = errorCode](const std::pair<UINT, const char *> &element) {
                    return element.first == code;
                }
            );
            return (it != errorMappingsPretty.end()) ? it->second : AppCore::I18N::UNKNOWN_ERROR_STRING;
        };
    };
}
