/**
  ******************************************************************************
  * @file           : port_gpio.h
  * @brief          : 为ads1292提供cs、start、rest、drdy、四个gpio引脚的接口函数
  * @details        : 应用程序初始化应用程序函数。
  * @author         : Charmander 有为电子科技 QQ: 228303760
  * @date           : 2020/9/2
  * @version        : V0.1
  * @copyright      : Copyright (C) 
  ******************************************************************************
  * @attention      
  * NONE
  ******************************************************************************
  * @verbatim
  * 修改历史:    
  *      1. 日期：2020/9/2
  *         作者：Charmander
  *         修改：
  * @endverbatim
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PORT_GPIO_H
#define __PORT_GPIO_H	  

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "port_typ.h"

#include "sys.h"

/** @defgroup PORT_GPIO PORT GPIO
  * @{
  */
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* 在此实现cs、start、rest、drdy、四个gpio引脚的控制接口宏 */
#define ADS1292_CS_SET()            PAout(3) = 1
#define ADS1292_CS_RESET()          PAout(3) = 0    
#define ADS1292_START_SET()         PAout(2) = 1
#define ADS1292_START_RESET()       PAout(2) = 0
#define ADS1292_REST_SET()          PAout(1) = 1
#define ADS1292_REST_RESET()        PAout(1) = 0
#define ADS1292_DRDY_GET()          PAin(4)

/* Exported functions --------------------------------------------------------*/
/** @addtogroup Port_Gpio_Exported_Functions Port Gpio Exported Functions 
  * @{
  */

/** @brief  微妙延时
  * @details 
  * @retval None
  */
void port_gpio_init(void) ;
/**
  * @}
  */
  
/**
  * @}
  */

#ifdef __cplusplus
}
#endif
  
#endif /* __PORT_GPIO_H */




