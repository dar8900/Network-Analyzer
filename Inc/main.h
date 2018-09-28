/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f1xx_hal.h"

/* Private define ------------------------------------------------------------*/

#define GeneralSensor_Pin GPIO_PIN_0
#define GeneralSensor_GPIO_Port GPIOA
#define CurrentSensor_Pin GPIO_PIN_1
#define CurrentSensor_GPIO_Port GPIOA
#define Red_Pin GPIO_PIN_3
#define Red_GPIO_Port GPIOA
#define Blue_Pin GPIO_PIN_4
#define Blue_GPIO_Port GPIOA
#define Green_Pin GPIO_PIN_5
#define Green_GPIO_Port GPIOA
#define Up_Pin GPIO_PIN_6
#define Up_GPIO_Port GPIOA
#define Down_Pin GPIO_PIN_7
#define Down_GPIO_Port GPIOA
#define Left_Pin GPIO_PIN_0
#define Left_GPIO_Port GPIOB
#define Right_Pin GPIO_PIN_1
#define Right_GPIO_Port GPIOB
#define Ok_Pin GPIO_PIN_2
#define Ok_GPIO_Port GPIOB
#define EnergyLed_Pin GPIO_PIN_10
#define EnergyLed_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
