/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Stm32ItmLoggerCPPWrapper.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ETH_HandleTypeDef heth;
extern TIM_HandleTypeDef htim14;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  // https://interrupt.memfault.com/blog/cortex-m-hardfault-debug
  // DUI0553

  __unused __IM uint32_t BFAR        = SCB->BFAR;
  __unused __IM uint32_t MMFAR       = SCB->MMFAR;

  __unused __IM uint8_t DIVBYZERO    = (SCB->CFSR & SCB_CFSR_DIVBYZERO_Msk) >> SCB_CFSR_DIVBYZERO_Pos;
  __unused __IM uint8_t UNALIGNED    = (SCB->CFSR & SCB_CFSR_UNALIGNED_Msk) >> SCB_CFSR_UNALIGNED_Pos;
  __unused __IM uint8_t NOCP         = (SCB->CFSR & SCB_CFSR_NOCP_Msk) >> SCB_CFSR_NOCP_Pos;
  __unused __IM uint8_t INVPC        = (SCB->CFSR & SCB_CFSR_INVPC_Msk) >> SCB_CFSR_INVPC_Pos;
  __unused __IM uint8_t INVSTATE     = (SCB->CFSR & SCB_CFSR_INVSTATE_Msk) >> SCB_CFSR_INVSTATE_Pos;
  __unused __IM uint8_t UNDEFINSTR   = (SCB->CFSR & SCB_CFSR_UNDEFINSTR_Msk) >> SCB_CFSR_UNDEFINSTR_Pos;

  __unused __IM uint8_t BFARVALID    = (SCB->CFSR & SCB_CFSR_BFARVALID_Msk) >> SCB_CFSR_BFARVALID_Pos;
  __unused __IM uint8_t LSPERR       = (SCB->CFSR & SCB_CFSR_LSPERR_Msk) >> SCB_CFSR_LSPERR_Pos;
  __unused __IM uint8_t STKERR       = (SCB->CFSR & SCB_CFSR_STKERR_Msk) >> SCB_CFSR_STKERR_Pos;
  __unused __IM uint8_t UNSTKERR     = (SCB->CFSR & SCB_CFSR_UNSTKERR_Msk) >> SCB_CFSR_UNSTKERR_Pos;
  __unused __IM uint8_t IMPRECISERR  = (SCB->CFSR & SCB_CFSR_IMPRECISERR_Msk) >> SCB_CFSR_IMPRECISERR_Pos;
  __unused __IM uint8_t PRECISERR    = (SCB->CFSR & SCB_CFSR_PRECISERR_Msk) >> SCB_CFSR_PRECISERR_Pos;
  __unused __IM uint8_t IBUSERR      = (SCB->CFSR & SCB_CFSR_IBUSERR_Msk) >> SCB_CFSR_IBUSERR_Pos;

  __unused __IM uint8_t MMARVALID    = (SCB->CFSR & SCB_CFSR_MMARVALID_Msk) ? 1 : 0;
  __unused __IM uint8_t MLSPERR      = (SCB->CFSR & SCB_CFSR_MLSPERR_Msk) ? 1 : 0;
  __unused __IM uint8_t MSTKERR      = (SCB->CFSR & SCB_CFSR_MSTKERR_Msk) ? 1 : 0;
  __unused __IM uint8_t MUNSTKERR    = (SCB->CFSR & SCB_CFSR_MUNSTKERR_Msk) ? 1 : 0;
  __unused __IM uint8_t DACCVIOL     = (SCB->CFSR & SCB_CFSR_DACCVIOL_Msk) ? 1 : 0;
  __unused __IM uint8_t IACCVIOL     = (SCB->CFSR & SCB_CFSR_IACCVIOL_Msk) ? 1 : 0;

  __unused __IM uint8_t DEBUGEVT     = (SCB->HFSR & SCB_HFSR_DEBUGEVT_Msk) ? 1 : 0;
  __unused __IM uint8_t FORCED       = (SCB->HFSR & SCB_HFSR_FORCED_Msk) ? 1 : 0;
  __unused __IM uint8_t VECTTBL      = (SCB->HFSR & SCB_HFSR_VECTTBL_Msk) ? 1 : 0;




  Logger_println("==> HardFault_Handler()");

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM8 trigger and commutation interrupts and TIM14 global interrupt.
  */
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 0 */

  /* USER CODE END TIM8_TRG_COM_TIM14_IRQn 0 */
  HAL_TIM_IRQHandler(&htim14);
  /* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 1 */

  /* USER CODE END TIM8_TRG_COM_TIM14_IRQn 1 */
}

/**
  * @brief This function handles Ethernet global interrupt.
  */
void ETH_IRQHandler(void)
{
  /* USER CODE BEGIN ETH_IRQn 0 */

  /* USER CODE END ETH_IRQn 0 */
  HAL_ETH_IRQHandler(&heth);
  /* USER CODE BEGIN ETH_IRQn 1 */

  /* USER CODE END ETH_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
