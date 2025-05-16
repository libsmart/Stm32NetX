/*
 * SPDX-FileCopyrightText: 2025 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#pragma once
#include "NetXReturnValues.hpp"
#include "Hash/Fnv1a.hpp"

extern "C" {
#include "nx_api.h"
}

using namespace Stm32Common::Hash;

namespace Stm32NetX {

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

    inline const char * NetXRuntimeException::what() const noexcept {
        return runtime_error::what();
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
