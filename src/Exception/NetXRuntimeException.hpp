/*
 * SPDX-FileCopyrightText: 2025 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#pragma once
#include "I18N/gettext_map.hpp"

extern "C" {
#include "nx_api.h"
}

namespace Stm32NetX {
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

#if __EXCEPTIONS
#include <stdexcept>
#define LIBSMART_HANDLE_NETX_ERROR(ret, fmt, ...)                       \
do {                                                                    \
char buffer[snprintf(nullptr, 0, fmt, __VA_ARGS__) + 1]{};              \
snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__);                     \
log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)->println(buffer); \
throw Stm32NetX::NetXRuntimeException(buffer, ret);                     \
} while (0);


    class NetXRuntimeException : public std::runtime_error {
    public:
        explicit NetXRuntimeException(const UINT errorCode) : NetXRuntimeException(
            "NetX runtime exception", errorCode) { ; }

        NetXRuntimeException(const char *string, UINT errorCode) : runtime_error(string), errorCode(errorCode) { ; }

        [[nodiscard]] virtual UINT getErrorCode() const { return errorCode; }

        [[nodiscard]] virtual const char *getErrorString() const noexcept;

        [[nodiscard]] const char *what() const noexcept override;

    private:
        const UINT errorCode;
    };

    inline const char *NetXRuntimeException::getErrorString() const noexcept {
        const auto it = std::find_if(
            NetXApiReturnValues::errorMappings.begin(),
            NetXApiReturnValues::errorMappings.end(),
            [code = errorCode](const std::pair<UINT, const char *> &element) {
                return element.first == code;
            }
        );
        return (it != NetXApiReturnValues::errorMappings.end()) ? it->second : runtime_error::what();
    }

    inline const char *NetXRuntimeException::what() const noexcept {
        const auto it = std::find_if(
            NetXApiReturnValues::errorMappingsPretty.begin(),
            NetXApiReturnValues::errorMappingsPretty.end(),
            [code = errorCode](const std::pair<UINT, const char *> &element) {
                return element.first == code;
            }
        );
        return (it != NetXApiReturnValues::errorMappingsPretty.end()) ? it->second : runtime_error::what();
    }


#else
#define LIBSMART_HANDLE_ERROR(ret, fmt, ...)                                          \
do {                                                                    \
char buffer[snprintf(nullptr, 0, fmt, __VA_ARGS__) + 1]{};              \
snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__);                     \
log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)->println(buffer); \
return ret;                                                             \
} while (0);
#endif
}
