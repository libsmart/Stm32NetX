/*
 * SPDX-FileCopyrightText: 2025 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include "NetXReturnValues.hpp"
#include "Result.hpp"
#include "Result/PrintableResult.hpp"

namespace Stm32NetX {
    using nxResult_t = PrintableResult<void, Stm32NetX::NetXResult>;
}
