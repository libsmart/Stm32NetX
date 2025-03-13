/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "Loggable.hpp"
#include "nx_api.h"

namespace Stm32NetX {
    class Address : public NXD_ADDRESS {
    public:
        Address() = default;

        Address(
            const uint8_t ipByte1,
            const uint8_t ipByte2,
            const uint8_t ipByte3,
            const uint8_t ipByte4
        ) : Address(IP_ADDRESS(ipByte1, ipByte2, ipByte3, ipByte4)) { ; }

        explicit Address(const ULONG addr) : NXD_ADDRESS_STRUCT() {
            nxd_ip_version = NX_IP_VERSION_V4;
            nxd_ip_address.v4 = addr;
        }

        explicit Address(NXD_ADDRESS *other)
            : NXD_ADDRESS(*other) { ; }

        Address(Address &&other) noexcept
            : NXD_ADDRESS(std::move(other)) { ; }

        Address &operator=(const Address &other) {
            if (this == &other) return *this;
            NXD_ADDRESS::operator =(other);
            return *this;
        }

        Address &operator=(Address &&other) noexcept {
            if (this == &other) return *this;
            NXD_ADDRESS::operator =(std::move(other));
            return *this;
        }

        Address &operator=(const char *str) {
            this->nxd_ip_version = 0;
            this->nxd_ip_address.v4 = 0;

            ULONG b1{}, b2{}, b3{}, b4{};
            if (sscanf(str, "%u.%u.%u.%u", &b1, &b2, &b3, &b4) != 4) {
                return *this;
            }
            this->nxd_ip_version = NX_IP_VERSION_V4;
            this->nxd_ip_address.v4 = static_cast<ULONG>(b1) << 24
                                      | static_cast<ULONG>(b2) << 16
                                      | static_cast<ULONG>(b3) << 8
                                      | static_cast<ULONG>(b4);

            return *this;
        }

        bool operator==(const Address &other) const {
            if (this->nxd_ip_version != other.nxd_ip_version) return false;
            if (this->nxd_ip_version == NX_IP_VERSION_V4) {
                if (this->nxd_ip_address.v4 != other.nxd_ip_address.v4) return false;
            }

            if (this->nxd_ip_version == NX_IP_VERSION_V6) {
#ifdef FEATURE_NX_IPV6
                if (this->nxd_ip_address.v6 != other.nxd_ip_address.v6) return false;
#else
                return false;
#endif
            }
            return true;
        }

        bool isValid() {
            return nxd_ip_version == NX_IP_VERSION_V4 || nxd_ip_version == NX_IP_VERSION_V6;
        }
    };
}
