#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "stm32f10x.h"

#define OLED_ADDRESS	0x78

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

/* STM32 I2C 快速模式 */
#define I2C_Speed              100000

/*I2C接口*/
#define OLED_I2C                          I2C1
#define OLED_I2C_CLK                      RCC_APB1Periph_I2C1
#define OLED_I2C_CLK_INIT				  RCC_APB1PeriphClockCmd

#define OLED_I2C_SCL_PIN                  GPIO_Pin_6
#define OLED_I2C_SCL_GPIO_PORT            GPIOB
#define OLED_I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOB
#define OLED_I2C_SCL_SOURCE               GPIO_PinSource6
#define OLED_I2C_SCL_AF                   GPIO_AF_I2C1

#define OLED_I2C_SDA_PIN                  GPIO_Pin_7
#define OLED_I2C_SDA_GPIO_PORT            GPIOB
#define OLED_I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOB
#define OLED_I2C_SDA_SOURCE               GPIO_PinSource7
#define OLED_I2C_SDA_AF                   GPIO_AF_I2C1


void I2C_Configuration(void);
void I2C_WriteByte(uint8_t addr,uint8_t data);

//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_Show_Wellcome(unsigned char x, unsigned char y, unsigned char N);//显示汉字：家庭健康监测系统
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void LCD_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 s);
void LCD_DrawCurve(s32 *data);
void OLED_ShowString(u8 x,u8 y, u8 *p,u8 Char_Size);

#endif
