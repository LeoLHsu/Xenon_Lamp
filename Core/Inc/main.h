/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Debug.h"
#include "define.h"
#include "SystemTimeBase.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC_CH0_D24V_Pin GPIO_PIN_0
#define ADC_CH0_D24V_GPIO_Port GPIOA
#define ADC_CH1_D5V_Pin GPIO_PIN_1
#define ADC_CH1_D5V_GPIO_Port GPIOA
#define ADC_CH2_LAMP_CURR_Pin GPIO_PIN_2
#define ADC_CH2_LAMP_CURR_GPIO_Port GPIOA
#define ADC_CH3_LAMP_VOL_Pin GPIO_PIN_3
#define ADC_CH3_LAMP_VOL_GPIO_Port GPIOA
#define DAC_CH0_LAMP_SET_Pin GPIO_PIN_4
#define DAC_CH0_LAMP_SET_GPIO_Port GPIOA
#define LAMP_EN_Pin GPIO_PIN_0
#define LAMP_EN_GPIO_Port GPIOB
#define UART_DEBUG_TX_Pin GPIO_PIN_9
#define UART_DEBUG_TX_GPIO_Port GPIOA
#define UART_DEBUG_RX_Pin GPIO_PIN_10
#define UART_DEBUG_RX_GPIO_Port GPIOA
#define CAN_HOST_RX_Pin GPIO_PIN_11
#define CAN_HOST_RX_GPIO_Port GPIOA
#define CAN_HOST_TX_Pin GPIO_PIN_12
#define CAN_HOST_TX_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_15
#define LED_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
