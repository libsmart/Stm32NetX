/*
 * SPDX-FileCopyrightText: 2025 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include "Stm32NetX.hpp"
#include "Exception/LibsmartException.hpp"
#include "Result/NetXResult.hpp"

#if __EXCEPTIONS
namespace Stm32NetX {
    class NetXException : public Stm32Common::LibsmartException {
    public:
        NetXException(const char *CLASS_NAME, const char *message)
            : LibsmartException(Stm32NetX::COMPONENT_NAME, CLASS_NAME, message) { ; }

        NetXException(const char *COMPONENT_NAME, const char *CLASS_NAME, const char *message)
            : LibsmartException(COMPONENT_NAME, CLASS_NAME, message) { ; }

        explicit NetXException(const nxResult_t &result)
            : LibsmartException(result.COMPONENT_NAME, result.CLASS_NAME, result.INSTANCE_NAME, result.FUNCTION_NAME,
                                result.isError() ? result.error().message() : ""), nxRes(result) { ; }

        NetXException(const char *COMPONENT_NAME, const char *CLASS_NAME, const char *INSTANCE_NAME,
                      const char *FUNCTION_NAME, const char *message)
            : LibsmartException(COMPONENT_NAME, CLASS_NAME, INSTANCE_NAME, FUNCTION_NAME, message) { ; }


        size_t printTo(Stm32Common::PrintInterface &printObject) const override {
            return printObject.print(nxRes);
        }

    private:
        nxResult_t nxRes = nxResult_t::err(Stm32NetX::NetXResult{});
    };
}
#endif
