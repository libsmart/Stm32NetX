/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32NETX_LIBSMART_CONFIG_DIST_HPP
#define LIBSMART_STM32NETX_LIBSMART_CONFIG_DIST_HPP

#define LIBSMART_STM32NETX

#include <main.h>
#include <nx_user.h>
#include <nx_api.h>

#include <nx_secure_tls_api.h>
#ifdef nx_secure_tls_initialize
#define LIBSMART_STM32NETX_ENABLE_TLS
#endif

#include <nxd_dns.h>
#ifdef NX_DNS_SUCCESS
#define LIBSMART_STM32NETX_ENABLE_DNS
#endif

#define LIBSMART_STM32NETX_DEFAULT_MEMORY_SIZE 1024
#define LIBSMART_STM32NETX_DEFAULT_PRIORITY 10

#define LIBSMART_STM32NETX_NETX_THREAD_STACK_SIZE (2*LIBSMART_STM32NETX_DEFAULT_MEMORY_SIZE)

#define LIBSMART_STM32NETX_NETX_LINK_CHECK_INTERVAL (3 * TX_TIMER_TICKS_PER_SECOND)
#define LIBSMART_STM32NETX_NETX_DHCP_WAIT_TIME (6 * TX_TIMER_TICKS_PER_SECOND)

#ifdef ETH_RX_BUF_SIZE
#define LIBSMART_STM32NETX_PAYLOAD_SIZE ETH_RX_BUF_SIZE
#else
#warning ETH_RX_BUF_SIZE is not set. Using default value of 1536
#define LIBSMART_STM32NETX_PAYLOAD_SIZE 1536
#endif

#define LIBSMART_STM32NETX_PACKET_POOL_SIZE ((LIBSMART_STM32NETX_PAYLOAD_SIZE + sizeof(NX_PACKET)) * 10)

#ifdef NX_ENABLE_DUAL_PACKET_POOL
#define LIBSMART_STM32NETX_PAYLOAD_AUX_SIZE 512
#define LIBSMART_STM32NETX_PACKET_POOL_AUX_SIZE ((LIBSMART_STM32NETX_PAYLOAD_AUX_SIZE + sizeof(NX_PACKET)) * 20)
#endif

#define LIBSMART_STM32NETX_DEFAULT_HOSTNAME "Stm32NetX"
#define LIBSMART_STM32NETX_DEFAULT_IP_ADDRESS IP_ADDRESS(192,168,1,1)
#define LIBSMART_STM32NETX_DEFAULT_NETMASK IP_ADDRESS(255,255,255,0)
#define LIBSMART_STM32NETX_DEFAULT_GATEWAY_ADDRESS IP_ADDRESS(0,0,0,0)
#define LIBSMART_STM32NETX_DEFAULT_DHCP true

#define LIBSMART_STM32NETX_ENABLE_DHCP
// #undef LIBSMART_STM32NETX_ENABLE_DHCP



#endif
