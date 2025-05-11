/*
 * SPDX-FileCopyrightText: 2025 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include "NetXReturnValues.hpp"
#include "Result.hpp"
#include "Result/Result.hpp"

namespace Stm32NetX {
    using nxResult_t = Stm32Common::Result<void, Stm32NetX::NetXResult>;
}
