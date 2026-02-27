/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define TL_SN_G_Pin GPIO_PIN_0
#define TL_SN_G_GPIO_Port GPIOC
#define TL_SN_Y_Pin GPIO_PIN_1
#define TL_SN_Y_GPIO_Port GPIOC
#define TL_WE_R_Pin GPIO_PIN_0
#define TL_WE_R_GPIO_Port GPIOA
#define TL_WE_Y_Pin GPIO_PIN_1
#define TL_WE_Y_GPIO_Port GPIOA
#define TL_WE_G_Pin GPIO_PIN_4
#define TL_WE_G_GPIO_Port GPIOA
#define TL_NS_G_Pin GPIO_PIN_5
#define TL_NS_G_GPIO_Port GPIOA
#define TL_NS_Y_Pin GPIO_PIN_6
#define TL_NS_Y_GPIO_Port GPIOA
#define TL_NS_R_Pin GPIO_PIN_7
#define TL_NS_R_GPIO_Port GPIOA
#define TL_SN_R_Pin GPIO_PIN_0
#define TL_SN_R_GPIO_Port GPIOB
#define TL_EW_R_Pin GPIO_PIN_13
#define TL_EW_R_GPIO_Port GPIOB
#define TL_EW_Y_Pin GPIO_PIN_14
#define TL_EW_Y_GPIO_Port GPIOB
#define TL_EW_G_Pin GPIO_PIN_15
#define TL_EW_G_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_15
#define CS_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
