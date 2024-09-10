#include "sys.h"
#include "delay.h"
#include "math.h"
#include "time.h"
#include "dma.h"
#include "usart.h"
#include "usart2.h"
#include "stdio.h"
#include "string.h"
#include "mpu6050.h"
#include "beep.h" 
#include "OLED_I2C.h"
#include "bsp_ads1115.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "led.h"
#include "adc.h"
//网络协议层
#include "onenet.h"
//网络设备
#include "esp8266.h"
#define Step 60

#pragma anon_unions
#pragma pack(push)
#pragma pack(1)

struct tx {

    int32_t ecg_buf[120];
    u8 Heart_rate;
    s16  temp;
    u16  count;
    u16  dis;
	
} tx_data;

#pragma pack(pop)

short aacx,aacy,aacz;		//加速度传感器原始数据
short gyrox,gyroy,gyroz;	//陀螺仪原始数据
unsigned long count;
u16 adcx2;//烟雾浓度原始值
float temp2;//烟雾浓度
float tem;//温度
int Heart_rate;
char display[20];
u8 txflag = 0;
u8 mpuerrflag = 0;
u8 mpu_flag = 0;      			//人体跌倒检测标志位 标志位1、2任意一个异常 该标志位为1 【1：跌倒，0：正常】
_Bool mpu_1_flag = 0;      		//人体跌倒检测标志位1 角度异常标志 【1：异常，0：正常】
_Bool mpu_2_flag = 0;      		//人体跌倒检测标志位2 加速度异常标志 【1：异常，0：正常】
int SVM;//人体加速度向量幅值
u8 i=3,t=0;

float pitch,roll,yaw; 	//欧拉角
void Net_Init()
{
	ESP8266_Init();					//初始化ESP8266
	
	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
		
}

//硬件初始化
void Hardware_Init(void)
{
		delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Usart1_Init(115200);
		usart2_init();
		Usart3_Init(115200); 	//串口2，驱动ESP8266用
		LED_Init();
    OLED_Init();
		BEEP_Init();
    MPU_Init();
    ads1115_Init();
		Adc_Init();
		Time4_Int_Init();
}

void fall(){
	
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据		
				SVM = sqrt(pow(aacx,2)+  pow(aacy,2) + pow(aacz,2));	
				//printf("pitch:%0.1f   roll:%0.1f   yaw:%0.1f   SVM:%u\r\n",fabs(pitch),fabs(roll),fabs(yaw),SVM);
				
				//分析x、y、z角度的异常判断
				if( fabs(pitch)>40 || fabs(roll)>40 || fabs(yaw)>40 )//倾斜角度的“绝对值”大于70°，SVM大于设定的阈值时，即认为摔倒
					mpu_1_flag = 1;	
				else 
					mpu_1_flag = 0;
				
				//分析加速度SVM的异常判断
				if( SVM>22000 || SVM<12000 ){i = 0;}
				i++;
				
				if( i<=3 ){mpu_2_flag = 1;}
				else 
				{
					i = 3;
					mpu_2_flag = 0; 
				}
		
				//综合欧拉角、SVM异常判断异常
				if( mpu_2_flag || mpu_1_flag){mpu_flag = 1;}
				else {mpu_flag = 0;}
				
				BEEP=0;		  
				delay_ms(300);//延时300ms
				if(mpu_flag==1)
				{
						BEEP=1; 
						LED2=0;
						LED1=1;
					delay_ms(300);//延时300ms
					
				}	BEEP=0;	
				delay_ms(300);
				LED1=0;
				LED2=1;

}
int main(void)
{
	u8 dmaable = 1;
    u16 adcx;
    float temp;
u8 j=0;
	unsigned char *dataPtr = NULL;
	unsigned short timeCount = 1000;	//发送间隔变量
  
	
		Hardware_Init();				//初始化外围硬件
				 temp2=0;
	
	//----------Welcome家庭健康检测系统-----------------------------------
		OLED_Clear();	
		OLED_ShowString(30,1,(u8*)"Welcome",7);
		for(j=0;j<7;j++)
				{
						OLED_Show_Wellcome(10+j*16,3,j);
				}
					delay_ms(1200);
					OLED_Clear();
	//---------------------------------------------
		OLED_ShowString(0,1,(u8*)"The network is  connecting,     please waite... ",50);	
		 Net_Init();						//网络初始化
    
   
    //MYDMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)&tx_data,sizeof(tx_data));//DMA1通道4,外设为串口1,存储器为SendBuff,长度SEND_BUF_SIZE.
	
	
		
	while(mpu_dmp_init())
    {
				static u8 cnt = 20;
				
        OLED_ShowString(0,0,(u8*)"MPU ERROR!!",12);
        delay_ms(200);
        OLED_ShowString(0,0,(u8*)"           ",12);
        delay_ms(200);
		if(!(cnt--))
		{
			mpuerrflag = 1;
			break;
		}
    }
		
    OLED_Clear();
		
		LED= LED_ON;						//入网成功
		
    Time3_Int_Init();
			LED2=1;
			LED1=0;
		
		
    while(1)
    {		
		
				BEEP=0;
			//--------------------------------------------------------
			//采集ADC数 (温度)
        adcx=GetAds1115Values();//采集ADC数据
        temp=(float)adcx*0.125f;
        tem = (-0.0000084515f)*temp*temp+(-0.176928f)*temp+203.393f;
        //步数
       // if(mpuerrflag == 0) dmp_get_pedometer_step_count(&count);
			mpu_flag = 0;
			OLED_ShowString(0,7,(u8*)display,12);
			LCD_DrawCurve(ecg_buf);
			
			//跌倒检测-------------------------------------------------------------
		  fall();
			
				
			
//--------------------------------------------------------------------------------------------------------
			OneNet_SendData();	//发送数据
			ESP8266_Clear();
//--------------------------------------------		
			 //本地显示
        
				//传输到onenet的变量
				Heart_rate=rx_data.Heart_rate;

			
			
  }
}
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {	
			
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
			
        txflag = 1;
    }
}

void TIM4_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {	
		
       
       TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
			
			adcx2=Get_Adc_Average(ADC_Channel_1,10);
		
			temp2=(float)adcx2*(3.3/4096);
			sprintf(display,"%d %.1f %.2lf %d",rx_data.Heart_rate,tem+0.05f, temp2,mpu_flag);
						//采集烟雾数值
			if(temp2>=0.75){
						BEEP=1;
				}
			
    }
}