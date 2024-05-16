/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Stm32NetXCPPWrapper.hpp"
#include <Stm32NetX.hpp>

UINT Stm32NetX_setup(TX_BYTE_POOL *byte_pool) {
    return Stm32NetX::NetX::setup(byte_pool);
}
