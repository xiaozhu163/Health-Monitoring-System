#include "OLED_I2C.h"
#include "oledfont.h"
#include "string.h"
#include "delay.h"



/**
 * @brief  I2C_Configuration，初始化硬件IIC引脚
 * @param  无
 * @retval 无
 */
void I2C_Configuration(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    /*I2C1外设时钟使能 */
    OLED_I2C_CLK_INIT(OLED_I2C_CLK,ENABLE);

    /*I2C1外设GPIO时钟使能 */
    RCC_APB2PeriphClockCmd(OLED_I2C_SCL_GPIO_CLK | OLED_I2C_SDA_GPIO_CLK,ENABLE);

    /* I2C_SCL、I2C_SDA*/
    GPIO_InitStructure.GPIO_Pin = OLED_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
    GPIO_Init(OLED_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = OLED_I2C_SDA_PIN;
    GPIO_Init(OLED_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);


    /* I2C 配置 */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;	/* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
    I2C_InitStructure.I2C_OwnAddress1 =OLED_ADDRESS;    //主机的I2C地址
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	/* I2C的寻址模式 */
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;	                            /* 通信速率 */

    I2C_Init(OLED_I2C, &I2C_InitStructure);	                                      /* I2C1 初始化 */
    I2C_Cmd(OLED_I2C, ENABLE);  	                                                /* 使能 I2C1 */

}


/**
 * @brief  I2C_WriteByte，向OLED寄存器地址写一个byte的数据
 * @param  addr：寄存器地址
*					data：要写入的数据
 * @retval 无
 */
void I2C_WriteByte(uint8_t addr,uint8_t data)
{
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);//开启I2C1
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/

    I2C_Send7bitAddress(I2C1, OLED_ADDRESS, I2C_Direction_Transmitter);//器件地址 -- 默认0x78
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, addr);//寄存器地址
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C1, data);//发送数据
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);//关闭I2C1总线
}


//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
    switch(cmd)
    {
    case 0:
        I2C_WriteByte(0x00, dat);
        break;
    case 1:
        I2C_WriteByte(0x40, dat);
        break;
    }

}

void WriteDat(unsigned char I2C_Data)//写数据
{
	I2C_WriteByte(0x40, I2C_Data);
}

/**
 * @brief  OLED_Init，初始化OLED
 * @param  无
 * @retval 无
 */
//初始化SSD1306
void OLED_Init(void)
{
    I2C_Configuration();

    delay_ms(500);
    OLED_WR_Byte(0xAE,OLED_CMD);//--display off
    OLED_WR_Byte(0x02,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address
    OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
    OLED_WR_Byte(0x81,OLED_CMD); // contract control
    OLED_WR_Byte(0xFF,OLED_CMD);//--128
    OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap
    OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
    OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
    OLED_WR_Byte(0x00,OLED_CMD);//

    OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
    OLED_WR_Byte(0x80,OLED_CMD);//

    OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
    OLED_WR_Byte(0x05,OLED_CMD);//

    OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
    OLED_WR_Byte(0xF1,OLED_CMD);//

    OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
    OLED_WR_Byte(0x12,OLED_CMD);//

    OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
    OLED_WR_Byte(0x30,OLED_CMD);//

    OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
    OLED_WR_Byte(0x14,OLED_CMD);//

    OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel

    OLED_Clear();
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
    u8 i,n;
    for(i=0; i<8; i++)
    {
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址
        for(n=0; n<128; n++)OLED_WR_Byte(0,OLED_DATA);
    } //更新显示
}
//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
    unsigned char j=0;
    while (chr[j]!='\0')
    {
        OLED_ShowChar(x,y,chr[j],Char_Size);
        x+=8;
        if(x>120)
        {
            x=0;
            y+=2;
        }
        j++;
    }
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
//size:选择字体 16/12
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{
    unsigned char c=0,i=0;
    c=chr-' ';//得到偏移后的值
    if(x>128-1) {
        x=0;
        y=y+2;
    }
    if(Char_Size ==16)
    {
        OLED_Set_Pos(x,y);
        for(i=0; i<8; i++)
            OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
        OLED_Set_Pos(x,y+1);
        for(i=0; i<8; i++)
            OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
    }
    else {
        OLED_Set_Pos(x,y);
        for(i=0; i<6; i++)
            OLED_WR_Byte(F6x8[c][i],OLED_DATA);

    }
}
//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f),OLED_CMD);
}
//开启OLED显示
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127
//定义上面7行用于曲线显示
u8 OLED_GRAM[128][7];

//更新显存到LCD
void OLED_Refresh_Gram(void)
{
    u8 i,n;
    for(i=0; i<7; i++)
    {
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址
        for(n=0; n<128; n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
    }
}
//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
    u8 pos,bx,temp=0;
    if(x>127||y>55)return;//超出范围了
    pos=6-y/8;
    bx=y%8;
    temp=1<<(7-bx);
    if(t)OLED_GRAM[x][pos]|=temp;
    else OLED_GRAM[x][pos]&=~temp;
}
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
//s:1 填充 0,清空
void LCD_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 s)
{
    u16 t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //计算坐标增量
    delta_y=y2-y1;
    uRow=x1;
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向
    else if(delta_x==0)incx=0;//垂直线
    else {
        incx=-1;
        delta_x=-delta_x;
    }
    if(delta_y>0)incy=1;
    else if(delta_y==0)incy=0;//水平线
    else {
        incy=-1;
        delta_y=-delta_y;
    }
    if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
    else distance=delta_y;
    for(t=0; t<=distance+1; t++ ) //画线输出
    {
        OLED_DrawPoint(uRow,uCol,s);//画点
        xerr+=delta_x ;
        yerr+=delta_y ;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}

//绘制心率曲线
void LCD_DrawCurve(s32 *data)
{
	u8 i = 0;
	s32 max = -5000000;
	s32 min = 5000000;
	static float k;
	
	for(;i<128;i++)
	{
		max = max < data[i] ? data[i] : max;
		min = min > data[i] ? data[i] : min;
	}
	
	k = (float)56/(max - min);
	
    memset(OLED_GRAM,0,sizeof(OLED_GRAM));
    for(i  = 0; i < 127 ; i++)
    {
        LCD_DrawLine(i,(data[i]-min)*k,i+1,(data[i+1]-min)*k,1);
    }
	OLED_Refresh_Gram();
}


void OLED_Show_Wellcome(unsigned char x, unsigned char y, unsigned char N)//显示中文点阵（中文需要先取模然后添加到字库中）
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(System[adder]);
		adder += 1;
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(System[adder]);
		adder += 1;
	}
}