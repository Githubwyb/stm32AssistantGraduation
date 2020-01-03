/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#define KEY_1_Pin GPIO_PIN_2
#define KEY_1_GPIO_Port GPIOE
#define KEY_2_Pin GPIO_PIN_3
#define KEY_2_GPIO_Port GPIOE
#define KEY_3_Pin GPIO_PIN_4
#define KEY_3_GPIO_Port GPIOE
#define FPGA_START_Pin GPIO_PIN_0
#define FPGA_START_GPIO_Port GPIOF
#define FPGA_START_EXTI_IRQn EXTI0_IRQn
#define FPGA_RST_Pin GPIO_PIN_1
#define FPGA_RST_GPIO_Port GPIOF
#define FPGA_RST_EXTI_IRQn EXTI1_IRQn
#define LED_0_Pin GPIO_PIN_0
#define LED_0_GPIO_Port GPIOC
#define LED_1_Pin GPIO_PIN_1
#define LED_1_GPIO_Port GPIOC
#define LED_2_Pin GPIO_PIN_2
#define LED_2_GPIO_Port GPIOC
#define LED_3_Pin GPIO_PIN_3
#define LED_3_GPIO_Port GPIOC
#define KEY_UP_Pin GPIO_PIN_0
#define KEY_UP_GPIO_Port GPIOA
#define FPGA_CS_Pin GPIO_PIN_4
#define FPGA_CS_GPIO_Port GPIOA
#define FPGA_SCK_Pin GPIO_PIN_5
#define FPGA_SCK_GPIO_Port GPIOA
#define FPGA_MOSI_Pin GPIO_PIN_7
#define FPGA_MOSI_GPIO_Port GPIOA
#define LED_4_Pin GPIO_PIN_4
#define LED_4_GPIO_Port GPIOC
#define LED_5_Pin GPIO_PIN_5
#define LED_5_GPIO_Port GPIOC
#define FLASH_SCK_Pin GPIO_PIN_13
#define FLASH_SCK_GPIO_Port GPIOB
#define FLASH_MISO_Pin GPIO_PIN_14
#define FLASH_MISO_GPIO_Port GPIOB
#define FLASH_MOSI_Pin GPIO_PIN_15
#define FLASH_MOSI_GPIO_Port GPIOB
#define LED_6_Pin GPIO_PIN_6
#define LED_6_GPIO_Port GPIOC
#define LED_7_Pin GPIO_PIN_7
#define LED_7_GPIO_Port GPIOC
#define FLASH_CS_Pin GPIO_PIN_13
#define FLASH_CS_GPIO_Port GPIOG
#define DDS_SCK_Pin GPIO_PIN_3
#define DDS_SCK_GPIO_Port GPIOB
#define DDS_MOSI_Pin GPIO_PIN_5
#define DDS_MOSI_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
