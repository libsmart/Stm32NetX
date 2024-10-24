/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

#include "Helper.hpp"
#include "RTE_Components.h"
#ifdef NX_ETH_INTERFACE_ENABLED
#warning Using default eth driver nx_stm32_eth_driver.c
#else

/* Indicate that driver source is being compiled.  */

#define NX_DRIVER_SOURCE

/****** DRIVER SPECIFIC ****** Start of part/vendor specific include area.  Include driver-specific include file here!  */

#ifndef NX_STM32_ETH_DRIVER_H

/* Determine if the driver uses IP deferred processing or direct ISR processing.  */

#define NX_DRIVER_ENABLE_DEFERRED                /* Define this to enable deferred ISR processing.  */

/* Include driver specific include file.  */
#include "nx_stm32_eth_driver.h"

/* Include the phy driver header */
#include "nx_stm32_phy_driver.h"

#endif /* NX_STM32_ETH_DRIVER_H */
#include "Stm32ItmLoggerCPPWrapper.hpp"

/****** DRIVER SPECIFIC ****** End of part/vendor specific include file area!  */


/* Define the driver information structure that is only available within this file.  */
/* Place Ethernet BD at uncacheable memory*/
static  NX_DRIVER_INFORMATION nx_driver_information;


extern ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
extern ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */


ETH_TxPacketConfig TxPacketCfg;
ETH_MACFilterConfigTypeDef FilterConfig;

/****** DRIVER SPECIFIC ****** Start of part/vendor specific data area.  Include hardware-specific data here!  */

/****** DRIVER SPECIFIC ****** End of part/vendor specific data area!  */


/* Define the routines for processing each driver entry request.  The contents of these routines will change with
each driver. However, the main driver entry function will not change, except for the entry function name.  */

static VOID         _nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_enable(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_disable(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_get_status(NX_IP_DRIVER *driver_req_ptr);
#ifdef NX_ENABLE_INTERFACE_CAPABILITY
static VOID         _nx_driver_capability_get(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_capability_set(NX_IP_DRIVER *driver_req_ptr);
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

static VOID         _nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr);

static VOID         _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr);


/* Define the prototypes for the hardware implementation of this driver. The contents of these routines are
driver-specific.  */

static UINT         _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr);
static UINT         _nx_driver_hardware_multicast_join(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_multicast_leave(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_get_status(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_hardware_packet_received(VOID);
#ifdef NX_ENABLE_INTERFACE_CAPABILITY
static UINT         _nx_driver_hardware_capability_set(NX_IP_DRIVER *driver_req_ptr);
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    nx_stm32_eth_driver                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This is the entry point of the NetX Ethernet Driver. This driver    */
/*    function is responsible for initializing the Ethernet controller,   */
/*    enabling or disabling the controller as need, preparing             */
/*    a packet for transmission, and getting status information.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        The driver request from the   */
/*                                            IP layer.                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_interface_attach           Process attach request        */
/*    _nx_driver_initialize                 Process initialize request    */
/*    _nx_driver_enable                     Process link enable request   */
/*    _nx_driver_disable                    Process link disable request  */
/*    _nx_driver_packet_send                Process send packet requests  */
/*    _nx_driver_multicast_join             Process multicast join request*/
/*    _nx_driver_multicast_leave            Process multicast leave req   */
/*    _nx_driver_get_status                 Process get status request    */
/*    _nx_driver_deferred_processing        Drive deferred processing     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    IP layer                                                            */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
/****** DRIVER SPECIFIC ****** Start of part/vendor specific global driver entry function name.  */
VOID  nx_stm32_eth_driver(NX_IP_DRIVER *driver_req_ptr)
/****** DRIVER SPECIFIC ****** End of part/vendor specific global driver entry function name.  */
{

  /* Default to successful return.  */
  driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

  /* Process according to the driver request type in the IP control
  block.  */
  switch (driver_req_ptr -> nx_ip_driver_command)
  {

  case NX_LINK_INTERFACE_ATTACH:

    /* Process link interface attach requests.  */
    _nx_driver_interface_attach(driver_req_ptr);
    break;

  case NX_LINK_INITIALIZE:
    {

      /* Process link initialize requests.  */
      _nx_driver_initialize(driver_req_ptr);
      break;
    }

  case NX_LINK_ENABLE:
    {

      /* Process link enable requests.  */
      _nx_driver_enable(driver_req_ptr);
      break;
    }

  case NX_LINK_DISABLE:
    {

      /* Process link disable requests.  */
      _nx_driver_disable(driver_req_ptr);
      break;
    }


  case NX_LINK_ARP_SEND:
  case NX_LINK_ARP_RESPONSE_SEND:
  case NX_LINK_PACKET_BROADCAST:
  case NX_LINK_RARP_SEND:
  case NX_LINK_PACKET_SEND:
    {

      /* Process packet send requests.  */
      _nx_driver_packet_send(driver_req_ptr);
      break;
    }


  case NX_LINK_MULTICAST_JOIN:
    {

      /* Process multicast join requests.  */
      _nx_driver_multicast_join(driver_req_ptr);
      break;
    }


  case NX_LINK_MULTICAST_LEAVE:
    {

      /* Process multicast leave requests.  */
      _nx_driver_multicast_leave(driver_req_ptr);
      break;
    }

  case NX_LINK_GET_STATUS:
    {

      /* Process get status requests.  */
      _nx_driver_get_status(driver_req_ptr);
      break;
    }

  case NX_LINK_DEFERRED_PROCESSING:
    {

      /* Process driver deferred requests.  */

      /* Process a device driver function on behave of the IP thread. */
      _nx_driver_deferred_processing(driver_req_ptr);

      break;
    }


#ifdef NX_ENABLE_INTERFACE_CAPABILITY
  case NX_INTERFACE_CAPABILITY_GET:
    {

      /* Process get capability requests.  */
      _nx_driver_capability_get(driver_req_ptr);
      break;
    }

  case NX_INTERFACE_CAPABILITY_SET:
    {

      /* Process set capability requests.  */
      _nx_driver_capability_set(driver_req_ptr);
      break;
    }
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

  default:


    /* Invalid driver request.  */

    /* Return the unhandled command status.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_UNHANDLED_COMMAND;

    /* Default to successful return.  */
    // driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }

    if(driver_req_ptr->nx_ip_driver_status != 0) {
        Logger_printf("ETH DRIVER ERROR: driver_req_ptr->nx_ip_driver_status = %d\r\n", (driver_req_ptr->nx_ip_driver_status));
        if(driver_req_ptr->nx_ip_driver_status == NX_SUCCESS /* 0x00 */) Logger_println("  NX_SUCCESS");
        if(driver_req_ptr->nx_ip_driver_status == NX_ALREADY_ENABLED /* 0x15 */) Logger_println("  NX_ALREADY_ENABLED");
        if(driver_req_ptr->nx_ip_driver_status == NX_ENTRY_NOT_FOUND /* 0x16 */) Logger_println("  NX_ENTRY_NOT_FOUND");
        if(driver_req_ptr->nx_ip_driver_status == NX_NO_MORE_ENTRIES /* 0x17 */) Logger_println("  NX_NO_MORE_ENTRIES");
        if(driver_req_ptr->nx_ip_driver_status == NX_UNHANDLED_COMMAND /* 0x44 */) Logger_println("  NX_UNHANDLED_COMMAND");
        if(driver_req_ptr->nx_ip_driver_status == NX_INVALID_INTERFACE /* 0x4c */) Logger_println("  NX_INVALID_INTERFACE");
        if(driver_req_ptr->nx_ip_driver_status == NX_DRIVER_ERROR /* 0x5a */) Logger_println("  NX_DRIVER_ERROR");

        Logger_printf("ETH DRIVER ERROR: driver_req_ptr->nx_ip_driver_command = %d\r\n", (driver_req_ptr->nx_ip_driver_command));
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_PACKET_SEND /* 0x00 */) Logger_println("  NX_LINK_PACKET_SEND");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_INITIALIZE /* 0x01 */) Logger_println("  NX_LINK_INITIALIZE");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_ENABLE /* 0x02 */) Logger_println("  NX_LINK_ENABLE");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_DISABLE /* 0x03 */) Logger_println("  NX_LINK_DISABLE");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_PACKET_BROADCAST /* 0x04 */) Logger_println("  NX_LINK_PACKET_BROADCAST");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_ARP_SEND /* 0x05 */) Logger_println("  NX_LINK_ARP_SEND");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND /* 0x06 */) Logger_println("  NX_LINK_ARP_RESPONSE_SEND");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_RARP_SEND /* 0x07 */) Logger_println("  NX_LINK_RARP_SEND");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_MULTICAST_JOIN /* 0x08 */) Logger_println("  NX_LINK_MULTICAST_JOIN");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_MULTICAST_LEAVE /* 0x09 */) Logger_println("  NX_LINK_MULTICAST_LEAVE");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_GET_STATUS /*0x0a */) Logger_println("  NX_LINK_GET_STATUS");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_DEFERRED_PROCESSING /* 0x12 */) Logger_println("  NX_LINK_DEFERRED_PROCESSING");
        if(driver_req_ptr->nx_ip_driver_command == NX_LINK_INTERFACE_ATTACH /* 0x13 */) Logger_println("  NX_LINK_INTERFACE_ATTACH");
        if(driver_req_ptr->nx_ip_driver_command == NX_INTERFACE_CAPABILITY_GET /* 0x15 */) Logger_println("  NX_INTERFACE_CAPABILITY_GET");
        if(driver_req_ptr->nx_ip_driver_command == NX_INTERFACE_CAPABILITY_SET /* 0x16 */) Logger_println("  NX_INTERFACE_CAPABILITY_SET");

        // assert_param(1==2);

    }

}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_interface_attach                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the interface attach request.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr)
{


  /* Setup the driver's interface.  This example is for a simple one-interface
  Ethernet driver. Additional logic is necessary for multiple port devices.  */
  nx_driver_information.nx_driver_information_interface =  driver_req_ptr -> nx_ip_driver_interface;

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
  driver_req_ptr -> nx_ip_driver_interface -> nx_interface_capability_flag = NX_DRIVER_CAPABILITY;
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

  /* Return successful status.  */
  driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_initialize                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the initialize request.  The processing    */
/*    in this function is generic. All ethernet controller logic is to    */
/*    be placed in _nx_driver_hardware_initialize.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_initialize        Process initialize request    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr)
{

  NX_IP           *ip_ptr;
  NX_INTERFACE    *interface_ptr;
  UINT            status;


  /* Setup the IP pointer from the driver request.  */
  ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

  /* Setup interface pointer.  */
  interface_ptr = driver_req_ptr -> nx_ip_driver_interface;

  /* Initialize the driver's information structure.  */

  /* Default IP pointer to NULL.  */
  nx_driver_information.nx_driver_information_ip_ptr =               NX_NULL;

  /* Setup the driver state to not initialized.  */
  nx_driver_information.nx_driver_information_state =                NX_DRIVER_STATE_NOT_INITIALIZED;

  /* Setup the default packet pool for the driver's received packets.  */
  nx_driver_information.nx_driver_information_packet_pool_ptr = ip_ptr -> nx_ip_default_packet_pool;

  /* Clear the deferred events for the driver.  */
  nx_driver_information.nx_driver_information_deferred_events =       0;

  /* Call the hardware-specific ethernet controller initialization.  */
  status =  _nx_driver_hardware_initialize(driver_req_ptr);

  /* Determine if the request was successful.  */
  if (status == NX_SUCCESS)
  {

    /* Successful hardware initialization.  */

    /* Setup driver information to point to IP pointer.  */
    nx_driver_information.nx_driver_information_ip_ptr = driver_req_ptr -> nx_ip_driver_ptr;

    /* Setup the link maximum transfer unit. */
    interface_ptr -> nx_interface_ip_mtu_size =  NX_DRIVER_ETHERNET_MTU - NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Setup the physical address of this IP instance.  Increment the
    physical address lsw to simulate multiple nodes hanging on the
    ethernet.  */
    interface_ptr -> nx_interface_physical_address_msw =
      (ULONG)(( eth_handle.Init.MACAddr[0] << 8) | ( eth_handle.Init.MACAddr[1]));
    interface_ptr -> nx_interface_physical_address_lsw =
      (ULONG)(( eth_handle.Init.MACAddr[2] << 24) | ( eth_handle.Init.MACAddr[3] << 16) |
              ( eth_handle.Init.MACAddr[4] << 8) | ( eth_handle.Init.MACAddr[5]));

    /* Indicate to the IP software that IP to physical mapping
    is required.  */
    interface_ptr -> nx_interface_address_mapping_needed =  NX_TRUE;

    /* Move the driver's state to initialized.  */
    nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;

    /* Indicate successful initialize.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
  else
  {

    /* Initialization failed.  Indicate that the request failed.  */
    driver_req_ptr -> nx_ip_driver_status =   NX_DRIVER_ERROR;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_enable                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the initialize request. The processing     */
/*    in this function is generic. All ethernet controller logic is to    */
/*    be placed in _nx_driver_hardware_enable.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_enable            Process enable request        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_enable(NX_IP_DRIVER *driver_req_ptr)
{

  NX_IP           *ip_ptr;
  ETH_MACConfigTypeDef MACConf;
  UINT            status, duplex, speed = 0;
  INT             PHYLinkState;

  /* Setup the IP pointer from the driver request.  */
  ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

  /* See if we can honor the NX_LINK_ENABLE request.  */
  if (nx_driver_information.nx_driver_information_state < NX_DRIVER_STATE_INITIALIZED)
  {

    /* Mark the request as not successful.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    return;
  }

  /* Check if it is enabled by someone already */
  if (nx_driver_information.nx_driver_information_state >=  NX_DRIVER_STATE_LINK_ENABLED)
  {

    /* Yes, the request has already been made.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_ALREADY_ENABLED;
    return;
  }

  if (nx_eth_phy_init() != ETH_PHY_STATUS_OK)
  {
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    return;
  }

  PHYLinkState = nx_eth_phy_get_link_state();

  /* Get link state */
  if(PHYLinkState <= ETH_PHY_STATUS_LINK_DOWN)
  {
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    return;
  }
  else
  {
    switch (PHYLinkState)
    {
    case ETH_PHY_STATUS_100MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case ETH_PHY_STATUS_100MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case ETH_PHY_STATUS_10MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    case ETH_PHY_STATUS_10MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    default:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    }

    /* Get MAC Config MAC */
    HAL_ETH_GetMACConfig(&eth_handle, &MACConf);
    MACConf.DuplexMode = duplex;
    MACConf.Speed = speed;
    HAL_ETH_SetMACConfig(&eth_handle, &MACConf);
  }

  /* Call hardware specific enable.  */
  status =  _nx_driver_hardware_enable(driver_req_ptr);

  /* Was the hardware enable successful?  */
  if (status == NX_SUCCESS)
  {

    /* Update the driver state to link enabled.  */
    nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_LINK_ENABLED;

    /* Mark request as successful.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

    /* Mark the IP instance as link up.  */
    ip_ptr -> nx_ip_driver_link_up =  NX_TRUE;
  }
  else
  {

    /* Enable failed.  Indicate that the request failed.  */
    driver_req_ptr -> nx_ip_driver_status =   NX_DRIVER_ERROR;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_disable                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the disable request. The processing        */
/*    in this function is generic. All ethernet controller logic is to    */
/*    be placed in _nx_driver_hardware_disable.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_disable           Process disable request       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_disable(NX_IP_DRIVER *driver_req_ptr)
{

  NX_IP           *ip_ptr;
  UINT            status;


  /* Setup the IP pointer from the driver request.  */
  ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

  /* Check if the link is enabled.  */
  if (nx_driver_information.nx_driver_information_state !=  NX_DRIVER_STATE_LINK_ENABLED)
  {

    /* The link is not enabled, so just return an error.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    return;
  }

  /* Call hardware specific disable.  */
  status =  _nx_driver_hardware_disable(driver_req_ptr);

  /* Was the hardware disable successful?  */
  if (status == NX_SUCCESS)
  {

    /* Mark the IP instance as link down.  */
    ip_ptr -> nx_ip_driver_link_up =  NX_FALSE;

    /* Update the driver state back to initialized.  */
    nx_driver_information.nx_driver_information_state =  NX_DRIVER_STATE_INITIALIZED;

    /* Mark request as successful.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
  else
  {

    /* Disable failed, return an error.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_packet_send                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the packet send request. The processing    */
/*    in this function is generic. All ethernet controller packet send    */
/*    logic is to be placed in _nx_driver_hardware_packet_send.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_packet_send       Process packet send request   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr)
{

  NX_IP           *ip_ptr;
  NX_PACKET       *packet_ptr;
  ULONG           *ethernet_frame_ptr;
  UINT            status;


  /* Setup the IP pointer from the driver request.  */
  ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

  /* Check to make sure the link is up.  */
  if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
  {

    /* Inidate an unsuccessful packet send.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;

    /* Link is not up, simply free the packet.  */
    nx_packet_transmit_release(driver_req_ptr -> nx_ip_driver_packet);

    Logger_println("nx_stm32_eth_driver.c::_nx_driver_packet_send(): LINK IS DOWN");
    return;
  }

  /* Process driver send packet.  */

  /* Place the ethernet frame at the front of the packet.  */
  packet_ptr =  driver_req_ptr -> nx_ip_driver_packet;

  /* Adjust the prepend pointer.  */
  packet_ptr -> nx_packet_prepend_ptr =
    packet_ptr -> nx_packet_prepend_ptr - NX_DRIVER_ETHERNET_FRAME_SIZE;

  /* Adjust the packet length.  */
  packet_ptr -> nx_packet_length = packet_ptr -> nx_packet_length + NX_DRIVER_ETHERNET_FRAME_SIZE;

  /* Setup the ethernet frame pointer to build the ethernet frame.  Backup another 2
  * bytes to get 32-bit word alignment.  */
  ethernet_frame_ptr =  (ULONG *) (packet_ptr -> nx_packet_prepend_ptr - 2);

  /* Set up the hardware addresses in the Ethernet header. */
  *ethernet_frame_ptr       =  driver_req_ptr -> nx_ip_driver_physical_address_msw;
  *(ethernet_frame_ptr + 1) =  driver_req_ptr -> nx_ip_driver_physical_address_lsw;

  *(ethernet_frame_ptr + 2) =  (ip_ptr -> nx_ip_arp_physical_address_msw << 16) |
    (ip_ptr -> nx_ip_arp_physical_address_lsw >> 16);
  *(ethernet_frame_ptr + 3) =  (ip_ptr -> nx_ip_arp_physical_address_lsw << 16);

  /* Set up the frame type field in the Ethernet harder. */
  if ((driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_SEND)||
      (driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND))
  {

    *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_ARP;
  }
  else if(driver_req_ptr -> nx_ip_driver_command == NX_LINK_RARP_SEND)
  {

    *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_RARP;
  }

#ifdef FEATURE_NX_IPV6
  else if(packet_ptr -> nx_packet_ip_version == NX_IP_VERSION_V6)
  {

    *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IPV6;
  }
#endif /* FEATURE_NX_IPV6 */

  else
  {

    *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IP;
  }

  /* Endian swapping if NX_LITTLE_ENDIAN is defined.  */
  NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr));
  NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 1));
  NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 2));
  NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 3));

  /* Determine if the packet exceeds the driver's MTU.  */
  if (packet_ptr -> nx_packet_length > NX_DRIVER_ETHERNET_MTU)
  {

    /* This packet exceeds the size of the driver's MTU. Simply throw it away! */
      Logger_println("nx_stm32_eth_driver.c::_nx_driver_packet_send(): packet exceeds the size of the driver's MTU");

    /* Remove the Ethernet header.  */
    NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

    /* Indicate an unsuccessful packet send.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;

    /* Link is not up, simply free the packet.  */
    nx_packet_transmit_release(packet_ptr);
    return;
  }

// while (eth_handle.gState == HAL_ETH_STATE_BUSY) {
    // delay(1);
// }

  /* Transmit the packet through the Ethernet controller low level access routine. */
  status = _nx_driver_hardware_packet_send(packet_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Driver's hardware send packet routine failed to send the packet.  */
      Logger_println("nx_stm32_eth_driver.c::_nx_driver_packet_send(): Driver's hardware send packet routine failed to send the packet");

    /* Remove the Ethernet header.  */
    NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

    /* Indicate an unsuccessful packet send.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;

    /* Link is not up, simply free the packet.  */
    nx_packet_transmit_release(packet_ptr);

      // assert_param(1==0);
  }
  else
  {

    /* Set the status of the request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_multicast_join                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the multicast join request. The processing */
/*    in this function is generic. All ethernet controller multicast join */
/*    logic is to be placed in _nx_driver_hardware_multicast_join.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_multicast_join    Process multicast join request*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr)
{

  UINT        status;


  /* Call hardware specific multicast join function. */
  status =  _nx_driver_hardware_multicast_join(driver_req_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Indicate an unsuccessful request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
  else
  {

    /* Indicate the request was successful.   */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_multicast_leave                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the multicast leave request. The           */
/*    processing in this function is generic. All ethernet controller     */
/*    multicast leave logic is to be placed in                            */
/*    _nx_driver_hardware_multicast_leave.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_multicast_leave   Process multicast leave req   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr)
{

  UINT        status;


  /* Call hardware specific multicast leave function. */
  status =  _nx_driver_hardware_multicast_leave(driver_req_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Indicate an unsuccessful request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
  else
  {

    /* Indicate the request was successful.   */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_get_status                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the get status request. The processing     */
/*    in this function is generic. All ethernet controller get status     */
/*    logic is to be placed in _nx_driver_hardware_get_status.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_get_status        Process get status request    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_get_status(NX_IP_DRIVER *driver_req_ptr)
{

  UINT        status;


  /* Call hardware specific get status function. */
  status =  _nx_driver_hardware_get_status(driver_req_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Indicate an unsuccessful request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
  else
  {

    /* Indicate the request was successful.   */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}


#ifdef NX_ENABLE_INTERFACE_CAPABILITY
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_capability_get                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the get capability request.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_capability_get(NX_IP_DRIVER *driver_req_ptr)
{

  /* Return the capability of the Ethernet controller.  */
  *(driver_req_ptr -> nx_ip_driver_return_ptr) = NX_DRIVER_CAPABILITY;

  /* Return the success status.  */
  driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_capability_set                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the set capability request.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_capability_set(NX_IP_DRIVER *driver_req_ptr)
{

  UINT        status;


  /* Call hardware specific get status function. */
  status =  _nx_driver_hardware_capability_set(driver_req_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Indicate an unsuccessful request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
  else
  {

    /* Indicate the request was successful.   */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_deferred_processing                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    XC, Microsoft Corporation                                           */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the deferred ISR action within the context */
/*    of the IP thread.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_packet_transmitted        Clean up after transmission    */
/*    _nx_driver_packet_received           Process a received packet      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr)
{

  TX_INTERRUPT_SAVE_AREA

    ULONG       deferred_events;


  /* Disable interrupts.  */
  TX_DISABLE

    /* Pickup deferred events.  */
    deferred_events =  nx_driver_information.nx_driver_information_deferred_events;
  nx_driver_information.nx_driver_information_deferred_events =  0;

  /* Restore interrupts.  */
  TX_RESTORE
    /* Check for a transmit complete event.  */
    if(deferred_events & NX_DRIVER_DEFERRED_PACKET_TRANSMITTED)
    {

      /* Process transmitted packet(s).  */
      HAL_ETH_ReleaseTxPacket(&eth_handle);
    }
  /* Check for received packet.  */
  if(deferred_events & NX_DRIVER_DEFERRED_PACKET_RECEIVED)
  {

    /* Process received packet(s).  */
    _nx_driver_hardware_packet_received();
  }

  /* Mark request as successful.  */
  driver_req_ptr->nx_ip_driver_status =  NX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_transfer_to_netx                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing incoming packets.  This routine would      */
/*    be called from the driver-specific receive packet processing        */
/*    function _nx_driver_hardware.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ip_ptr                                Pointer to IP protocol block  */
/*    packet_ptr                            Packet pointer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Error indication                                                    */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_ip_packet_receive                 NetX IP packet receive        */
/*    _nx_ip_packet_deferred_receive        NetX IP packet receive        */
/*    _nx_arp_packet_deferred_receive       NetX ARP packet receive       */
/*    _nx_rarp_packet_deferred_receive      NetX RARP packet receive      */
/*    _nx_packet_release                    Release packet                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_hardware_packet_received   Driver packet receive function*/
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr)
{

  USHORT    packet_type;


  /* Set the interface for the incoming packet.  */
  packet_ptr -> nx_packet_ip_interface = nx_driver_information.nx_driver_information_interface;

  /* Pickup the packet header to determine where the packet needs to be
  sent.  */
  packet_type =  (USHORT)(((UINT) (*(packet_ptr -> nx_packet_prepend_ptr+12))) << 8) |
    ((UINT) (*(packet_ptr -> nx_packet_prepend_ptr+13)));

  /* Route the incoming packet according to its ethernet type.  */
  if (packet_type == NX_DRIVER_ETHERNET_IP || packet_type == NX_DRIVER_ETHERNET_IPV6)
  {
    /* Note:  The length reported by some Ethernet hardware includes
    bytes after the packet as well as the Ethernet header.  In some
    cases, the actual packet length after the Ethernet header should
    be derived from the length in the IP header (lower 16 bits of
    the first 32-bit word).  */

    /* Clean off the Ethernet header.  */
    packet_ptr -> nx_packet_prepend_ptr += NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Adjust the packet length.  */
    packet_ptr -> nx_packet_length -= NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Route to the ip receive function.  */
    _nx_ip_packet_deferred_receive(ip_ptr, packet_ptr);
  }
  else if (packet_type == NX_DRIVER_ETHERNET_ARP)
  {

    /* Clean off the Ethernet header.  */
    packet_ptr -> nx_packet_prepend_ptr += NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Adjust the packet length.  */
    packet_ptr -> nx_packet_length -= NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Route to the ARP receive function.  */
    _nx_arp_packet_deferred_receive(ip_ptr, packet_ptr);
  }
  else if (packet_type == NX_DRIVER_ETHERNET_RARP)
  {

    /* Clean off the Ethernet header.  */
    packet_ptr -> nx_packet_prepend_ptr += NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Adjust the packet length.  */
    packet_ptr -> nx_packet_length -= NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Route to the RARP receive function.  */
    _nx_rarp_packet_deferred_receive(ip_ptr, packet_ptr);
  }
  else
  {
    /* Invalid ethernet header... release the packet.  */
    nx_packet_release(packet_ptr);
  }
}


/****** DRIVER SPECIFIC ****** Start of part/vendor specific internal driver functions.  */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_initialize                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific initialization.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    ETH_BSP_Config                        Configure Ethernet            */
/*    ETH_MACAddressConfig                  Setup MAC address             */
/*    ETH_DMARxDescReceiveITConfig          Enable receive descriptors    */
/*    nx_packet_allocate                    Allocate receive packet(s)    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_initialize                 Driver initialize processing  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr)
{

  /* Default to successful return.  */
  driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

  /* Setup indices.  */
  nx_driver_information.nx_driver_information_receive_current_index = 0;
  nx_driver_information.nx_driver_information_transmit_current_index = 0;
  nx_driver_information.nx_driver_information_transmit_release_index = 0;

  /* Clear the number of buffers in use counter.  */
  nx_driver_information.nx_driver_information_number_of_transmit_buffers_in_use = 0;

  /* Make sure there are receive packets... otherwise, return an error.  */
  if (nx_driver_information.nx_driver_information_packet_pool_ptr == NULL)
  {

    /* There must be receive packets. If not, return an error!  */
    return(NX_DRIVER_ERROR);
  }

#ifdef NX_DRIVER_ETH_HW_IP_INIT
  nx_eth_init();
#endif /* NX_DRIVER_ETH_HW_IP_INIT */

  ETH_DMAConfigTypeDef dmaDefaultConf;
  dmaDefaultConf.DMAArbitration = ETH_DMAARBITRATION_RX1_TX1;
  dmaDefaultConf.AddressAlignedBeats = ENABLE;
  dmaDefaultConf.BurstMode = ETH_BURSTLENGTH_FIXED;
  dmaDefaultConf.TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
  dmaDefaultConf.RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
  dmaDefaultConf.FlushRxPacket = DISABLE;
#ifndef STM32_ETH_HAL_LEGACY
  dmaDefaultConf.PBLx8Mode = DISABLE;
  dmaDefaultConf.RebuildINCRxBurst = DISABLE;
  dmaDefaultConf.SecondPacketOperate = ENABLE;
  dmaDefaultConf.TCPSegmentation = DISABLE;
  dmaDefaultConf.MaximumSegmentSize = 536;
#endif
#ifdef STM32_ETH_HAL_LEGACY
  dmaDefaultConf.DropTCPIPChecksumErrorFrame = ENABLE;
  dmaDefaultConf.ReceiveStoreForward =  DISABLE;
  dmaDefaultConf.TransmitStoreForward =  ENABLE;
  dmaDefaultConf.TransmitThresholdControl =  ENABLE;
  dmaDefaultConf.ForwardErrorFrames =  DISABLE;
  dmaDefaultConf.ReceiveThresholdControl =  DISABLE;
  dmaDefaultConf.SecondFrameOperate =  DISABLE;
  dmaDefaultConf.EnhancedDescriptorFormat =  DISABLE;
  dmaDefaultConf.DescriptorSkipLength =  DISABLE;
#endif
  /* enable OSF bit to enhance throughput */
  HAL_ETH_SetDMAConfig(&eth_handle, &dmaDefaultConf);

  FilterConfig.PromiscuousMode = DISABLE;
  FilterConfig.HashUnicast = DISABLE;
  FilterConfig.HashMulticast = DISABLE;
  FilterConfig.DestAddrInverseFiltering = DISABLE;
  FilterConfig.PassAllMulticast = DISABLE;
  FilterConfig.BroadcastFilter = ENABLE;
  FilterConfig.SrcAddrInverseFiltering = DISABLE;
  FilterConfig.SrcAddrFiltering = DISABLE;
  FilterConfig.HachOrPerfectFilter = DISABLE;
  FilterConfig.ReceiveAllMode = DISABLE;
  FilterConfig.ControlPacketsFilter = 0x00;

  /* Set Tx packet config common parameters */
  memset(&TxPacketCfg, 0, sizeof(ETH_TxPacketConfig));
  TxPacketCfg.Attributes = ETH_TX_PACKETS_FEATURES_CSUM ;
  TxPacketCfg.CRCPadCtrl = ETH_CRC_PAD_DISABLE;

  /* Clear the number of buffers in use counter.  */
  nx_driver_information.nx_driver_information_multicast_count = 0;

  /* Return success!  */
  return(NX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_enable                                          */
/*                                                            6.1         */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific link enable requests.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    ETH_Start                             Start Ethernet operation      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_enable                     Driver link enable processing */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr)
{

  /* Call STM32 library to start Ethernet operation.  */
  HAL_ETH_Start_IT(&eth_handle);

  /* Return success!  */
  return(NX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_disable                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific link disable requests.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    ETH_MACTransmissionCmd                Disable transmit              */
/*    ETH_FlushTransmitFIFO                 Flush transmit FIFO           */
/*    ETH_MACReceptionCmd                   Disable receive               */
/*    ETH_DMATransmissionCmd                Stop DMA transmission         */
/*    ETH_DMAReceptionCmd                   Stop DMA reception            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_disable                    Driver link disable processing*/
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr)
{

  HAL_ETH_Stop(&eth_handle);

  /* Return success!  */
  return(NX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_packet_send                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific packet send requests.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    packet_ptr                            Pointer to packet to send     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [_nx_driver_transmit_packet_enqueue]  Optional internal transmit    */
/*                                            packet queue routine        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_packet_send                Driver packet send processing */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr)
{

  NX_PACKET       *pktIdx;
  UINT            buffLen = 0;

  ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];
  memset(Txbuffer, 0 , ETH_TX_DESC_CNT*sizeof(ETH_BufferTypeDef));


  int i = 0;

  for (pktIdx = packet_ptr;pktIdx != NX_NULL ; pktIdx = pktIdx -> nx_packet_next)
  {
    if (i >= ETH_TX_DESC_CNT)
    {
        Logger_println("nx_stm32_eth_driver.c::_nx_driver_hardware_packet_send(): i >= ETH_TX_DESC_CNT");
      return NX_DRIVER_ERROR;
    }

    Txbuffer[i].buffer = pktIdx->nx_packet_prepend_ptr;
    Txbuffer[i].len = (pktIdx -> nx_packet_append_ptr - pktIdx->nx_packet_prepend_ptr);
    buffLen += (pktIdx -> nx_packet_append_ptr - pktIdx->nx_packet_prepend_ptr);

    if(i>0)
    {
      Txbuffer[i-1].next = &Txbuffer[i];
    }

    if (pktIdx-> nx_packet_next ==NULL)
    {
      Txbuffer[i].next = NULL;
    }

    i++;
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanDCache_by_Addr((uint32_t*)(pktIdx -> nx_packet_data_start), pktIdx -> nx_packet_data_end - pktIdx -> nx_packet_data_start);
#endif
  }

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
  if (packet_ptr -> nx_packet_interface_capability_flag & (NX_INTERFACE_CAPABILITY_TCP_TX_CHECKSUM |
                                                           NX_INTERFACE_CAPABILITY_UDP_TX_CHECKSUM |
                                                             NX_INTERFACE_CAPABILITY_ICMPV4_TX_CHECKSUM |
                                                               NX_INTERFACE_CAPABILITY_ICMPV6_TX_CHECKSUM))
  {
    TxPacketCfg.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  }
  else if (packet_ptr -> nx_packet_interface_capability_flag & NX_INTERFACE_CAPABILITY_IPV4_TX_CHECKSUM)
  {
    TxPacketCfg.ChecksumCtrl = ETH_CHECKSUM_IPHDR_INSERT;
  }
#else
  TxPacketCfg.ChecksumCtrl = ETH_CHECKSUM_DISABLE;
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

  TxPacketCfg.Length = buffLen;
  TxPacketCfg.TxBuffer = Txbuffer;
  TxPacketCfg.pData = (uint32_t *)packet_ptr;

  if(HAL_ETH_Transmit_IT(&eth_handle, &TxPacketCfg))
  {
      Logger_printf("nx_stm32_eth_driver.c::_nx_driver_hardware_packet_send(): HAL_ETH_Transmit_IT() ErrorCode=0x%04x\r\n", eth_handle.ErrorCode);
    return(NX_DRIVER_ERROR);
  }

  return(NX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_multicast_join                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific multicast join requests.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_multicast_join             Driver multicast join         */
/*                                            processing                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_multicast_join(NX_IP_DRIVER *driver_req_ptr)
{


  /* Increase the multicast count.  */
  nx_driver_information.nx_driver_information_multicast_count++;

  /* Enable multicast frame reception.  */
  FilterConfig.PassAllMulticast = ENABLE;
  HAL_ETH_SetMACFilterConfig(&eth_handle, &FilterConfig);

  /* Return success.  */
  return(NX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_multicast_leave                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific multicast leave requests. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]*/
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_multicast_leave            Driver multicast leave        */
/*                                            processing                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_multicast_leave(NX_IP_DRIVER *driver_req_ptr)
{

  /* Decrease the multicast count.  */
  nx_driver_information.nx_driver_information_multicast_count--;

  /* If multicast count reaches zero, disable multicast frame reception.  */
  if (nx_driver_information.nx_driver_information_multicast_count == 0)
  {

    /* Disable multicast frame reception.  */
    FilterConfig.PassAllMulticast = DISABLE;
    HAL_ETH_SetMACFilterConfig(&eth_handle, &FilterConfig);
  }

  /* Return success.  */
  return(NX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_get_status                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific get status requests.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]*/
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_get_status                 Driver get status processing  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_get_status(NX_IP_DRIVER *driver_req_ptr)
{
  INT PHYLinkState;

  /* Get link status. */
  PHYLinkState = nx_eth_phy_get_link_state();

  /* Check link status. */
  if(PHYLinkState <= ETH_PHY_STATUS_LINK_DOWN)
  {
    /* Update Link status if phsical link is down. */
    *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_FALSE;
  }
  else
  {
    /* Update Link status if phsical link is up. */
    *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_TRUE;
  }

  /* Return success. */
  return NX_SUCCESS;
}

void HAL_ETH_TxFreeCallback(uint32_t * buff)
{
  NX_PACKET * release_packet = (NX_PACKET *) buff;

  /* Remove the Ethernet header and release the packet.  */
  NX_DRIVER_ETHERNET_HEADER_REMOVE(release_packet);

  /* Release the packet.  */
  nx_packet_transmit_release(release_packet);
}

static VOID  _nx_driver_hardware_packet_received(VOID)
{
  NX_PACKET  *received_packet_ptr;

  while (HAL_ETH_ReadData(&eth_handle, (void **)&received_packet_ptr) == HAL_OK)
  {
      /* Transfer the packet to NetX.  */
      _nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, received_packet_ptr);
  }
}

void HAL_ETH_RxAllocateCallback(uint8_t ** buff)
{
  NX_PACKET     *packet_ptr;
  if (nx_packet_allocate(nx_driver_information.nx_driver_information_packet_pool_ptr, &packet_ptr,
                         NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS)
  {
    /* Adjust the packet.  */
    packet_ptr -> nx_packet_prepend_ptr += 2;
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_InvalidateDCache_by_Addr((uint32_t*)packet_ptr -> nx_packet_data_start, packet_ptr -> nx_packet_data_end - packet_ptr -> nx_packet_data_start);
#endif
    *buff = packet_ptr -> nx_packet_prepend_ptr;
  }
  else
  {
    /* Rx Buffer Pool is exhausted. */
    *buff = NULL;
  }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                                              */
/*                                                                        */
/*    HAL_ETH_RxLinkCallback                                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function chains all received packets before passing            */
/*    the first one to NetXDuo stack.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    first_packet_ptr           pointer to first received packet         */
/*    last_packet_ptr            pointer to last received packet          */
/*    buff                       pointer to received data                 */
/*    length                     received data length                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    received packet pointer                                             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    HAL_ETH_ReadData              Read a received packet                */
/*                                                                        */
/**************************************************************************/

void HAL_ETH_RxLinkCallback(void **first_packet_ptr, void **last_packet_ptr, uint8_t *buff, uint16_t Length)
{
  NX_PACKET **first_nx_packet_ptr = (NX_PACKET **)first_packet_ptr;
  NX_PACKET **last_nx_packet_ptr = (NX_PACKET **)last_packet_ptr;
  NX_PACKET  *received_packet_ptr;

  /* Indicate the offset of the received data.  */
  uint8_t *data_buffer_ptr = buff - 2U - sizeof(NX_PACKET);

  received_packet_ptr = (NX_PACKET *)data_buffer_ptr;
  received_packet_ptr->nx_packet_append_ptr = received_packet_ptr->nx_packet_prepend_ptr + Length;
  received_packet_ptr->nx_packet_length = Length;

  /* Check whether this is the first packet. */
  if (*first_nx_packet_ptr == NULL)
  {
    /* Add the first buffer of the packet. */
    *first_nx_packet_ptr = received_packet_ptr;
  }
  /* This is not the first packet. */
  else
  {
    /* Add the rest of the buffer to the end of the packet. */
    (*last_nx_packet_ptr)->nx_packet_next = received_packet_ptr;
  }
  /* Save the current packet in order to use it in the next iteration. */
  *last_nx_packet_ptr  = received_packet_ptr;
}

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_capability_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific capability set requests.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                         Driver request pointer       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_capability_set             Capability set processing     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_capability_set(NX_IP_DRIVER *driver_req_ptr)
{

  return NX_SUCCESS;
}
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */


void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{

  ULONG deffered_events;
  deffered_events = nx_driver_information.nx_driver_information_deferred_events;

  nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_RECEIVED;

  if (!deffered_events)
  {
    /* Call NetX deferred driver processing.  */
    _nx_ip_driver_deferred_processing(nx_driver_information.nx_driver_information_ip_ptr);
  }
}

void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
  ULONG deffered_events;
  deffered_events = nx_driver_information.nx_driver_information_deferred_events;


  nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_TRANSMITTED;

  if (!deffered_events)
  {
    /* Call NetX deferred driver processing.  */
    _nx_ip_driver_deferred_processing(nx_driver_information.nx_driver_information_ip_ptr);
  }
}

void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
{
    Logger_printf("ETH ERROR: heth->gState = 0x%08x\r\n", heth->gState);
    if((heth->gState | 0x00000000U) == 0) Logger_println("  HAL_ETH_STATE_RESET: Peripheral not yet Initialized or disabled");
    if((heth->gState & 0x00000010U) == 0x00000010U) Logger_println("  HAL_ETH_STATE_READY: Peripheral Communication started");
    if((heth->gState & 0x00000023U) == 0x00000023U) Logger_println("  HAL_ETH_STATE_BUSY: an internal process is ongoing");
    if((heth->gState & 0x00000023U) == 0x00000023U) Logger_println("  HAL_ETH_STATE_STARTED: an internal process is started");
    if((heth->gState & 0x000000E0U) == 0x000000E0U) Logger_println("  HAL_ETH_STATE_ERROR: Error State");

    Logger_printf("ETH ERROR: heth->ErrorCode = 0x%08x\r\n", heth->ErrorCode);
    if((heth->ErrorCode | 0x00000000U) == 0) Logger_println("  HAL_ETH_ERROR_NONE: No error");
    if((heth->ErrorCode & 0x00000001U) != 0) Logger_println("  HAL_ETH_ERROR_PARAM: Parameter error");
    if((heth->ErrorCode & 0x00000002U) != 0) Logger_println("  HAL_ETH_ERROR_BUSY: Busy error");
    if((heth->ErrorCode & 0x00000004U) != 0) Logger_println("  HAL_ETH_ERROR_TIMEOUT: Timeout error");
    if((heth->ErrorCode & 0x00000008U) != 0) Logger_println("  HAL_ETH_ERROR_DMA: DMA transfer error");
    if((heth->ErrorCode & 0x00000010U) != 0) Logger_println("  HAL_ETH_ERROR_MAC: MAC transfer error");
    if((heth->ErrorCode & 0x00000020U) != 0) Logger_println("  HAL_ETH_ERROR_INVALID_CALLBACK: Invalid Callback error");

    Logger_printf("ETH ERROR: heth->DMAErrorCode = 0x%08x\r\n", heth->DMAErrorCode);
    if((heth->DMAErrorCode | 0x00000000U) == 0) Logger_println("  ETH_DMA_RX_NO_ERROR_FLAG");
    if((heth->DMAErrorCode | 0x00000000U) == 0) Logger_println("  ETH_DMA_TX_NO_ERROR_FLAG");
    if((heth->DMAErrorCode & 0x00380000U) != 0) Logger_println("  ETH_DMA_RX_DESC_READ_ERROR_FLAG");
    if((heth->DMAErrorCode & 0x00300000U) != 0) Logger_println("  ETH_DMA_RX_DESC_WRITE_ERROR_FLAG");
    if((heth->DMAErrorCode & 0x00280000U) != 0) Logger_println("  ETH_DMA_RX_BUFFER_READ_ERROR_FLAG");
    if((heth->DMAErrorCode & 0x00200000U) != 0) Logger_println("  ETH_DMA_RX_BUFFER_WRITE_ERROR_FLAG");
    if((heth->DMAErrorCode & 0x00070000U) != 0) Logger_println("  ETH_DMA_TX_DESC_READ_ERROR_FLAG");
    if((heth->DMAErrorCode & 0x00060000U) != 0) Logger_println("  ETH_DMA_TX_DESC_WRITE_ERROR_FLAG");
    if((heth->DMAErrorCode & 0x00050000U) != 0) Logger_println("  ETH_DMA_TX_BUFFER_READ_ERROR_FLAG");
    if((heth->DMAErrorCode & 0x00040000U) != 0) Logger_println("  ETH_DMA_TX_BUFFER_WRITE_ERROR_FLAG");
    if((heth->DMAErrorCode & 0x00002000U) != 0) {
        Logger_println("  Fatal bus error");
        if((heth->DMAErrorCode & 0x00800000U) != 0) Logger_println("    23");
        if((heth->DMAErrorCode & 0x01000000U) != 0) Logger_println("    24");
        if((heth->DMAErrorCode & 0x02000000U) != 0) Logger_println("    25");
    }

    /*
    if((heth->DMAErrorCode & ETH_DMA_CONTEXT_DESC_ERROR_FLAG) != 0) Logger_println("ETH_DMA_CONTEXT_DESC_ERROR_FLAG");
    if((heth->DMAErrorCode & ETH_DMA_FATAL_BUS_ERROR_FLAG) != 0) Logger_println("ETH_DMA_FATAL_BUS_ERROR_FLAG");
    if((heth->DMAErrorCode & ETH_DMA_EARLY_TX_IT_FLAG) != 0) Logger_println("ETH_DMA_EARLY_TX_IT_FLAG");
    if((heth->DMAErrorCode & ETH_DMA_RX_WATCHDOG_TIMEOUT_FLAG) != 0) Logger_println("ETH_DMA_RX_WATCHDOG_TIMEOUT_FLAG");
    if((heth->DMAErrorCode & ETH_DMA_RX_PROCESS_STOPPED_FLAG) != 0) Logger_println("ETH_DMA_RX_PROCESS_STOPPED_FLAG");
    if((heth->DMAErrorCode & ETH_DMA_RX_BUFFER_UNAVAILABLE_FLAG) != 0) Logger_println("ETH_DMA_RX_BUFFER_UNAVAILABLE_FLAG");
    if((heth->DMAErrorCode & ETH_DMA_TX_PROCESS_STOPPED_FLAG) != 0) Logger_println("ETH_DMA_TX_PROCESS_STOPPED_FLAG");
    */

    Logger_printf("ETH ERROR: heth->MACErrorCode = 0x%08x\r\n", heth->MACErrorCode);
    if((heth->MACErrorCode | 0x00000000U) == 0) Logger_println("  NO_ERROR");
    /*
    if((heth->MACErrorCode & ETH_RECEIVE_WATCHDOG_TIMEOUT) != 0) Logger_println("ETH_RECEIVE_WATCHDOG_TIMEOUT");
    if((heth->MACErrorCode & ETH_EXECESSIVE_COLLISIONS) != 0) Logger_println("ETH_EXECESSIVE_COLLISIONS");
    if((heth->MACErrorCode & ETH_LATE_COLLISIONS) != 0) Logger_println("ETH_LATE_COLLISIONS");
    if((heth->MACErrorCode & ETH_EXECESSIVE_DEFERRAL) != 0) Logger_println("ETH_EXECESSIVE_DEFERRAL");
    if((heth->MACErrorCode & ETH_LOSS_OF_CARRIER) != 0) Logger_println("ETH_LOSS_OF_CARRIER");
    if((heth->MACErrorCode & ETH_NO_CARRIER) != 0) Logger_println("ETH_NO_CARRIER");
    if((heth->MACErrorCode & ETH_TRANSMIT_JABBR_TIMEOUT) != 0) Logger_println("ETH_TRANSMIT_JABBR_TIMEOUT");
    */

    assert_param(heth->gState != HAL_ETH_STATE_ERROR);
}


/****** DRIVER SPECIFIC ****** Start of part/vendor specific internal driver functions.  */
#endif
