/*
 * SPDX-FileCopyrightText: 2025 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <libsmart_config.hpp>
#include <climits>
#include <optional>
#include "Hash/Fnv1a.hpp"
#include "Enum/EnumRegistry.hpp"

extern "C" {
#include "nx_api.h"
}

namespace Stm32NetX {
    using namespace Stm32Common;
    using namespace Stm32Common::Hash;

    struct NetXReturn {
        enum class Id : UINT {
            NONE = UINT_MAX,
            SUCCESS = NX_SUCCESS,
            NO_PACKET = NX_NO_PACKET,
            UNDERFLOW = NX_UNDERFLOW,
            OVERFLOW = NX_OVERFLOW,
            NO_MAPPING = NX_NO_MAPPING,
            DELETED = NX_DELETED,
            POOL_ERROR = NX_POOL_ERROR,
            PTR_ERROR = NX_PTR_ERROR,
            WAIT_ERROR = NX_WAIT_ERROR,
            SIZE_ERROR = NX_SIZE_ERROR,
            OPTION_ERROR = NX_OPTION_ERROR,
            DELETE_ERROR = NX_DELETE_ERROR,
            CALLER_ERROR = NX_CALLER_ERROR,
            INVALID_PACKET = NX_INVALID_PACKET,
            INVALID_SOCKET = NX_INVALID_SOCKET,
            NOT_ENABLED = NX_NOT_ENABLED,
            ALREADY_ENABLED = NX_ALREADY_ENABLED,
            ENTRY_NOT_FOUND = NX_ENTRY_NOT_FOUND,
            NO_MORE_ENTRIES = NX_NO_MORE_ENTRIES,
            ARP_TIMER_ERROR = NX_ARP_TIMER_ERROR,
            RESERVED_CODE0 = NX_RESERVED_CODE0,
            WAIT_ABORTED = NX_WAIT_ABORTED,
            IP_INTERNAL_ERROR = NX_IP_INTERNAL_ERROR,
            IP_ADDRESS_ERROR = NX_IP_ADDRESS_ERROR,
            ALREADY_BOUND = NX_ALREADY_BOUND,
            PORT_UNAVAILABLE = NX_PORT_UNAVAILABLE,
            NOT_BOUND = NX_NOT_BOUND,
            RESERVED_CODE1 = NX_RESERVED_CODE1,
            SOCKET_UNBOUND = NX_SOCKET_UNBOUND,
            NOT_CREATED = NX_NOT_CREATED,
            SOCKETS_BOUND = NX_SOCKETS_BOUND,
            NO_RESPONSE = NX_NO_RESPONSE,
            POOL_DELETED = NX_POOL_DELETED,
            ALREADY_RELEASED = NX_ALREADY_RELEASED,
            RESERVED_CODE2 = NX_RESERVED_CODE2,
            MAX_LISTEN = NX_MAX_LISTEN,
            DUPLICATE_LISTEN = NX_DUPLICATE_LISTEN,
            NOT_CLOSED = NX_NOT_CLOSED,
            NOT_LISTEN_STATE = NX_NOT_LISTEN_STATE,
            IN_PROGRESS = NX_IN_PROGRESS,
            NOT_CONNECTED = NX_NOT_CONNECTED,
            WINDOW_OVERFLOW = NX_WINDOW_OVERFLOW,
            ALREADY_SUSPENDED = NX_ALREADY_SUSPENDED,
            DISCONNECT_FAILED = NX_DISCONNECT_FAILED,
            STILL_BOUND = NX_STILL_BOUND,
            NOT_SUCCESSFUL = NX_NOT_SUCCESSFUL,
            UNHANDLED_COMMAND = NX_UNHANDLED_COMMAND,
            NO_FREE_PORTS = NX_NO_FREE_PORTS,
            INVALID_PORT = NX_INVALID_PORT,
            INVALID_RELISTEN = NX_INVALID_RELISTEN,
            CONNECTION_PENDING = NX_CONNECTION_PENDING,
            TX_QUEUE_DEPTH = NX_TX_QUEUE_DEPTH,
            NOT_IMPLEMENTED = NX_NOT_IMPLEMENTED,
            NOT_SUPPORTED = NX_NOT_SUPPORTED,
            INVALID_INTERFACE = NX_INVALID_INTERFACE,
            INVALID_PARAMETERS = NX_INVALID_PARAMETERS,
            NOT_FOUND = NX_NOT_FOUND,
            CANNOT_START = NX_CANNOT_START,
            NO_INTERFACE_ADDRESS = NX_NO_INTERFACE_ADDRESS,
            INVALID_MTU_DATA = NX_INVALID_MTU_DATA,
            DUPLICATED_ENTRY = NX_DUPLICATED_ENTRY,
            PACKET_OFFSET_ERROR = NX_PACKET_OFFSET_ERROR,
            OPTION_HEADER_ERROR = NX_OPTION_HEADER_ERROR,
            CONTINUE = NX_CONTINUE,
            TCPIP_OFFLOAD_ERROR = NX_TCPIP_OFFLOAD_ERROR
        };

        struct Meta {
            Id id;
            UINT apiId;
#if defined(LIBSMART_ENABLE_ERROR_HASHES)
            FNV1a::fnv1a32_t strHash;
#endif
#if defined(LIBSMART_ENABLE_ERROR_STRINGS)
            const char *name;
#endif
        };

#if defined(LIBSMART_ENABLE_ERROR_HASHES)
#define IF_HASHES(...) , __VA_ARGS__
#else
#define IF_HASHES(...)
#endif

#if defined(LIBSMART_ENABLE_ERROR_STRINGS)
#define IF_STRINGS(...) , __VA_ARGS__
#else
#define IF_STRINGS(...)
#endif


#define DATA_LINE(x) { Id::x, NX_##x \
IF_HASHES(FNV1a::hash("NX_" #x)) \
IF_STRINGS("NX_" #x) \
}

        static constexpr Meta data[] = {
            {
                Id::NONE, UINT_MAX
#if defined(LIBSMART_ENABLE_ERROR_HASHES)
                , FNV1a::hash("NONE")
#endif
#if defined(LIBSMART_ENABLE_ERROR_STRINGS)
                , "NONE"
#endif
            },
            DATA_LINE(SUCCESS),
            DATA_LINE(NO_PACKET),
            DATA_LINE(UNDERFLOW),
            DATA_LINE(OVERFLOW),
            DATA_LINE(NO_MAPPING),
            DATA_LINE(DELETED),
            DATA_LINE(POOL_ERROR),
            DATA_LINE(PTR_ERROR),
            DATA_LINE(WAIT_ERROR),
            DATA_LINE(SIZE_ERROR),
            DATA_LINE(OPTION_ERROR),
            DATA_LINE(DELETE_ERROR),
            DATA_LINE(CALLER_ERROR),
            DATA_LINE(INVALID_PACKET),
            DATA_LINE(INVALID_SOCKET),
            DATA_LINE(NOT_ENABLED),
            DATA_LINE(ALREADY_ENABLED),
            DATA_LINE(ENTRY_NOT_FOUND),
            DATA_LINE(NO_MORE_ENTRIES),
            DATA_LINE(ARP_TIMER_ERROR),
            DATA_LINE(RESERVED_CODE0),
            DATA_LINE(WAIT_ABORTED),
            DATA_LINE(IP_INTERNAL_ERROR),
            DATA_LINE(IP_ADDRESS_ERROR),
            DATA_LINE(ALREADY_BOUND),
            DATA_LINE(PORT_UNAVAILABLE),
            DATA_LINE(NOT_BOUND),
            DATA_LINE(RESERVED_CODE1),
            DATA_LINE(SOCKET_UNBOUND),
            DATA_LINE(NOT_CREATED),
            DATA_LINE(SOCKETS_BOUND),
            DATA_LINE(NO_RESPONSE),
            DATA_LINE(POOL_DELETED),
            DATA_LINE(ALREADY_RELEASED),
            DATA_LINE(RESERVED_CODE2),
            DATA_LINE(MAX_LISTEN),
            DATA_LINE(DUPLICATE_LISTEN),
            DATA_LINE(NOT_CLOSED),
            DATA_LINE(NOT_LISTEN_STATE),
            DATA_LINE(IN_PROGRESS),
            DATA_LINE(NOT_CONNECTED),
            DATA_LINE(WINDOW_OVERFLOW),
            DATA_LINE(ALREADY_SUSPENDED),
            DATA_LINE(DISCONNECT_FAILED),
            DATA_LINE(STILL_BOUND),
            DATA_LINE(NOT_SUCCESSFUL),
            DATA_LINE(UNHANDLED_COMMAND),
            DATA_LINE(NO_FREE_PORTS),
            DATA_LINE(INVALID_PORT),
            DATA_LINE(INVALID_RELISTEN),
            DATA_LINE(CONNECTION_PENDING),
            DATA_LINE(TX_QUEUE_DEPTH),
            DATA_LINE(NOT_IMPLEMENTED),
            DATA_LINE(NOT_SUPPORTED),
            DATA_LINE(INVALID_INTERFACE),
            DATA_LINE(INVALID_PARAMETERS),
            DATA_LINE(NOT_FOUND),
            DATA_LINE(CANNOT_START),
            DATA_LINE(NO_INTERFACE_ADDRESS),
            DATA_LINE(INVALID_MTU_DATA),
            DATA_LINE(DUPLICATED_ENTRY),
            DATA_LINE(PACKET_OFFSET_ERROR),
            DATA_LINE(OPTION_HEADER_ERROR),
            DATA_LINE(CONTINUE),
            DATA_LINE(TCPIP_OFFLOAD_ERROR)
        };

        static constexpr EnumRegistry registry{data};

        using value = RegistryView<Meta>;

        static constexpr auto extractApiId(const Meta &e) { return e.apiId; };

        static constexpr std::optional<value> find(const UINT code) {
            auto a = registry.find<UINT, extractApiId>(code);
            if (a.has_value()) return a.value();
            return std::nullopt;
        }

        static constexpr auto extractId(const Meta &e) { return e.id; };

        static constexpr std::optional<value> find(const Id code) {
            auto a = registry.find<Id, extractId>(code);
            if (a.has_value()) return a.value();
            return std::nullopt;
        }
    };

    struct NetXResult : public Printable {
        NetXResult() { ; }

        template<typename E>
        explicit NetXResult(const E e) : _v(e) { ; }

        NetXResult(const NetXReturn::Meta &e) : _v(e) { apiId = e.apiId; msg = e.name; }
        NetXResult(const UINT e) : apiId(e) { _v.apiId = e; }
        NetXResult(const char *e) : msg(e) { ; }


        virtual UINT apiReturnValue() const {
            return _v.apiId;
        }

#if defined(LIBSMART_ENABLE_ERROR_HASHES)
        virtual FNV1a::fnv1a32_t strHash() const {
            return _v.strHash;
        }
#endif

#if defined(LIBSMART_ENABLE_ERROR_STRINGS)
        virtual const char *apiReturnString() const {
            return _v.name;
        }
#endif

        virtual const char *message() const {
            return msg;
        }

        const NetXReturn::Meta &raw() const { return _v; }

        size_t printTo(PrintInterface &printObject) const override {
            return printObject.printf(" = 0x%02x %s", _v.apiId, _v.name);
        }

    private:
        NetXReturn::Meta _v = NetXReturn::data[0];
        UINT apiId = UINT_MAX;
        const char *msg = "";
    };
}
