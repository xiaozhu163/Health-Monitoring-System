/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : 在此包含mcu芯片头文件
  * @details        : 在此包含mcu芯片头文件
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* 在此包含mcu芯片头文件 */
#include "stm32f10x.h"

  extern float tem;
	extern  int Heart_rate;
	extern  u8 mpu_flag;
	extern  float temp2;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

