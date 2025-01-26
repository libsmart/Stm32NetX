/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "Address.hpp"
#include "Printable.hpp"

#ifdef FEATURE_NX_IPV6
#define LIBSMART_STM32NETX_ADDRESS_STRING_SIZE 42
#else
#define LIBSMART_STM32NETX_ADDRESS_STRING_SIZE 16
#endif

namespace Stm32NetX {
    /**
     * @brief The AddressWriter class is responsible for converting an IP address to a string format and printing it.
     *
     * This class extends Stm32Common::Printable to allow instances to be printed using the Print interface.
     * It is a protected subclass of Address, utilizing its data to generate string representations of the IP address.
     */
    class AddressWriter : public Stm32Common::Printable, protected Address {
    public:
        explicit AddressWriter(Address &address)
            : Address(&address) { updateString(); }

        /**
         * @brief Prints the object by casting it to a const char pointer.
         *
         * This method overrides the printTo function from the Stm32Common::Printable interface.
         * It converts the current object instance to a string representation by casting it to
         * a const char pointer and then prints it using the provided printObject.
         *
         * @return The number of bytes written.
         */
        size_t printTo(Stm32Common::PrintInterface &printObject) const override {
            return printObject.print(reinterpret_cast<const char *>(this));
        }

        /**
         * @brief Converts the IP address to a string representation.
         *
         * This operator allows the conversion of the IP address to a const char pointer,
         * providing a string representation of the IP address contained in the instance.
         *
         * @return A const char pointer representing the IP address as a string.
         */
        explicit operator const char *() const {
            return ipAsString;
        }

        /**
         * @brief Updates the string representation of the IP address.
         *
         * This method converts the IP address stored in the instance into a string format.
         * For an IPv4 address, it converts the address to the dotted-decimal format (e.g., "192.168.1.1").
         * For an IPv6 address, it converts the address to the hexadecimal colon-separated format
         * (e.g., "[2001:0db8:85a3:0000:0000:8a2e:0370:7334]").
         * The result is stored in the ipAsString member variable.
         *
         * @warning The method is marked as `const`, but it uses `const_cast` to modify the ipAsString member variable,
         * which is not typically recommended. The use of `const_cast` should be reviewed to ensure it does not violate
         * the const-correctness of the class design.
         */
    protected:
        void updateString() {
            std::memset(const_cast<char *>(ipAsString), 0, sizeof(ipAsString));
            if (nxd_ip_version == NX_IP_VERSION_V4) {
                snprintf(const_cast<char *>(ipAsString), sizeof(ipAsString), "%u.%u.%u.%u",
                         nxd_ip_address.v4 >> 24,
                         nxd_ip_address.v4 >> 16 & 0xFF,
                         nxd_ip_address.v4 >> 8 & 0xFF,
                         nxd_ip_address.v4 & 0xFF
                );
            }
#ifdef FEATURE_NX_IPV6
            if (nxd_ip_version == NX_IP_VERSION_V6) {
                snprintf(const_cast<char *>(ipAsString), sizeof(ipAsString),
                         "[%x:%x:%x:%x:%x:%x:%x:%x]",
                         (nxd_ip_address.v6[0] >> 16) & 0xFFFF,
                         nxd_ip_address.v6[0] & 0xFFFF,
                         (nxd_ip_address.v6[1] >> 16) & 0xFFFF,
                         nxd_ip_address.v6[1] & 0xFFFF,
                         (nxd_ip_address.v6[2] >> 16) & 0xFFFF,
                         nxd_ip_address.v6[2] & 0xFFFF,
                         (nxd_ip_address.v6[3] >> 16) & 0xFFFF,
                         nxd_ip_address.v6[3] & 0xFFFF);
            }
#endif
        }

        const char ipAsString[LIBSMART_STM32NETX_ADDRESS_STRING_SIZE]{};
    };
}
