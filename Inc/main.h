//#define IND_BACKLIGHT 1		/* ≈сли определено, то дублируем вспышки светодиода на подсветку экрана */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#define APPLICATION_ADDRESS    0x08010000
//#define VERSION_ADDRESS (APPLICATION_ADDRESS + 0x10000)
#define VERSION_ADDRESS (APPLICATION_ADDRESS + 0x7000)
#define CHECKSUM_ADDRESS (VERSION_ADDRESS + 0x10)

#define LED_ERROR_PORT GPIOB
#define LED_ERROR_PIN GPIO_PIN_3
#define LED_ERROR(x) ((x)?(HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_RESET)));

#define LED_OK_PORT GPIOB
#define LED_OK_PIN GPIO_PIN_4
#define LED_OK(x) ((x)?(HAL_GPIO_WritePin(LED_OK_PORT,LED_OK_PIN,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(LED_OK_PORT,LED_OK_PIN,GPIO_PIN_RESET)));

#define USB_POWER_PORT GPIOC
#define USB_POWER_PIN GPIO_PIN_6
#define USB_POWER(x) ((x)?(HAL_GPIO_WritePin(USB_POWER_PORT,USB_POWER_PIN,GPIO_PIN_RESET)) : (HAL_GPIO_WritePin(USB_POWER_PORT,USB_POWER_PIN,GPIO_PIN_SET)));

#define USB_FLASH_FLAG_PORT GPIOC
#define USB_FLASH_FLAG_PIN GPIO_PIN_13
#define USB_FLASH_FLAG(x) ((x)?(HAL_GPIO_WritePin(USB_FLASH_FLAG_PORT,USB_FLASH_FLAG_PIN,GPIO_PIN_RESET)) : (HAL_GPIO_WritePin(USB_FLASH_FLAG_PORT,USB_FLASH_FLAG_PIN,GPIO_PIN_SET)));

#define BAT_ENABLE_PORT GPIOA
#define BAT_ENABLE_PIN GPIO_PIN_6
#define BAT_COMP HAL_GPIO_ReadPin(BAT_ENABLE_PORT,BAT_ENABLE_PIN)
#define BAT_ENABLE(x) ((x == 0)? (HAL_GPIO_WritePin(BAT_ENABLE_PORT,BAT_ENABLE_PIN,GPIO_PIN_RESET)) : (HAL_GPIO_WritePin(BAT_ENABLE_PORT,BAT_ENABLE_PIN,GPIO_PIN_SET)));

#define BUTTON2_PORT GPIOF
#define BUTTON2_PIN GPIO_PIN_11
#define ADM40100 (HAL_GPIO_ReadPin(BUTTON2_PORT,BUTTON2_PIN))

/*
#define PER_LED(x) ((x)? (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET)));

#ifdef IND_BACKLIGHT
 #define VLED_EN(x) ((x)? (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_RESET)));
#else
 #define VLED_EN(x)
#endif
*/

#define BUT_L HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_3)

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

#define	FILE_NOT_FOUND 0
#define	USB_NO_CONNECT 1
#define	FOUND_SAME_FIRMWARE 2
#define	FOUND_NEW_FIRMWARE 3

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
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
