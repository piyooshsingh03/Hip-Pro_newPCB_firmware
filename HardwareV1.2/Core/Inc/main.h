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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern UART_HandleTypeDef huart1;
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
#define EN_MICRO_Pin GPIO_PIN_0
#define EN_MICRO_GPIO_Port GPIOC
#define Charging_Voltage_Pin GPIO_PIN_1
#define Charging_Voltage_GPIO_Port GPIOC
#define PB_IN_MICRO_Pin GPIO_PIN_2
#define PB_IN_MICRO_GPIO_Port GPIOC
#define Sleep_N20_1_Pin GPIO_PIN_3
#define Sleep_N20_1_GPIO_Port GPIOC
#define IMU_INT_Pin GPIO_PIN_0
#define IMU_INT_GPIO_Port GPIOA
#define IMU_INT_EXTI_IRQn EXTI0_IRQn
#define CS_Pin GPIO_PIN_4
#define CS_GPIO_Port GPIOC
#define Sleep_N20_2_Pin GPIO_PIN_5
#define Sleep_N20_2_GPIO_Port GPIOC
#define SW1_Pin GPIO_PIN_0
#define SW1_GPIO_Port GPIOB
#define BAT_MEAS_EN_Pin GPIO_PIN_1
#define BAT_MEAS_EN_GPIO_Port GPIOB
#define N20_1__Pin GPIO_PIN_12
#define N20_1__GPIO_Port GPIOB
#define N20_1_B13_Pin GPIO_PIN_13
#define N20_1_B13_GPIO_Port GPIOB
#define N20_2__Pin GPIO_PIN_14
#define N20_2__GPIO_Port GPIOB
#define N20_2_B15_Pin GPIO_PIN_15
#define N20_2_B15_GPIO_Port GPIOB
#define Charging_STATUS_Pin GPIO_PIN_6
#define Charging_STATUS_GPIO_Port GPIOC
#define LOW_BAT_LED_Pin GPIO_PIN_8
#define LOW_BAT_LED_GPIO_Port GPIOA
#define FULL_BAT_LED_Pin GPIO_PIN_11
#define FULL_BAT_LED_GPIO_Port GPIOA
#define Power_Range_Pin GPIO_PIN_15
#define Power_Range_GPIO_Port GPIOA
#define SW2_Pin GPIO_PIN_4
#define SW2_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_5
#define BUZZER_GPIO_Port GPIOB
#define FAN_1_Pin GPIO_PIN_8
#define FAN_1_GPIO_Port GPIOB
#define FAN_2_Pin GPIO_PIN_9
#define FAN_2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
