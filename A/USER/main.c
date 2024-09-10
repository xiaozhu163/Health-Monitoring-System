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
//����Э���
#include "onenet.h"
//�����豸
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

short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����
unsigned long count;
u16 adcx2;//����Ũ��ԭʼֵ
float temp2;//����Ũ��
float tem;//�¶�
int Heart_rate;
char display[20];
u8 txflag = 0;
u8 mpuerrflag = 0;
u8 mpu_flag = 0;      			//�����������־λ ��־λ1��2����һ���쳣 �ñ�־λΪ1 ��1��������0��������
_Bool mpu_1_flag = 0;      		//�����������־λ1 �Ƕ��쳣��־ ��1���쳣��0��������
_Bool mpu_2_flag = 0;      		//�����������־λ2 ���ٶ��쳣��־ ��1���쳣��0��������
int SVM;//������ٶ�������ֵ
u8 i=3,t=0;

float pitch,roll,yaw; 	//ŷ����
void Net_Init()
{
	ESP8266_Init();					//��ʼ��ESP8266
	
	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
		
}

//Ӳ����ʼ��
void Hardware_Init(void)
{
		delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Usart1_Init(115200);
		usart2_init();
		Usart3_Init(115200); 	//����2������ESP8266��
		LED_Init();
    OLED_Init();
		BEEP_Init();
    MPU_Init();
    ads1115_Init();
		Adc_Init();
		Time4_Int_Init();
}

void fall(){
	
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������		
				SVM = sqrt(pow(aacx,2)+  pow(aacy,2) + pow(aacz,2));	
				//printf("pitch:%0.1f   roll:%0.1f   yaw:%0.1f   SVM:%u\r\n",fabs(pitch),fabs(roll),fabs(yaw),SVM);
				
				//����x��y��z�Ƕȵ��쳣�ж�
				if( fabs(pitch)>40 || fabs(roll)>40 || fabs(yaw)>40 )//��б�Ƕȵġ�����ֵ������70�㣬SVM�����趨����ֵʱ������Ϊˤ��
					mpu_1_flag = 1;	
				else 
					mpu_1_flag = 0;
				
				//�������ٶ�SVM���쳣�ж�
				if( SVM>22000 || SVM<12000 ){i = 0;}
				i++;
				
				if( i<=3 ){mpu_2_flag = 1;}
				else 
				{
					i = 3;
					mpu_2_flag = 0; 
				}
		
				//�ۺ�ŷ���ǡ�SVM�쳣�ж��쳣
				if( mpu_2_flag || mpu_1_flag){mpu_flag = 1;}
				else {mpu_flag = 0;}
				
				BEEP=0;		  
				delay_ms(300);//��ʱ300ms
				if(mpu_flag==1)
				{
						BEEP=1; 
						LED2=0;
						LED1=1;
					delay_ms(300);//��ʱ300ms
					
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
	unsigned short timeCount = 1000;	//���ͼ������
  
	
		Hardware_Init();				//��ʼ����ΧӲ��
				 temp2=0;
	
	//----------Welcome��ͥ�������ϵͳ-----------------------------------
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
		 Net_Init();						//�����ʼ��
    
   
    //MYDMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)&tx_data,sizeof(tx_data));//DMA1ͨ��4,����Ϊ����1,�洢��ΪSendBuff,����SEND_BUF_SIZE.
	
	
		
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
		
		LED= LED_ON;						//�����ɹ�
		
    Time3_Int_Init();
			LED2=1;
			LED1=0;
		
		
    while(1)
    {		
		
				BEEP=0;
			//--------------------------------------------------------
			//�ɼ�ADC�� (�¶�)
        adcx=GetAds1115Values();//�ɼ�ADC����
        temp=(float)adcx*0.125f;
        tem = (-0.0000084515f)*temp*temp+(-0.176928f)*temp+203.393f;
        //����
       // if(mpuerrflag == 0) dmp_get_pedometer_step_count(&count);
			mpu_flag = 0;
			OLED_ShowString(0,7,(u8*)display,12);
			LCD_DrawCurve(ecg_buf);
			
			//�������-------------------------------------------------------------
		  fall();
			
				
			
//--------------------------------------------------------------------------------------------------------
			OneNet_SendData();	//��������
			ESP8266_Clear();
//--------------------------------------------		
			 //������ʾ
        
				//���䵽onenet�ı���
				Heart_rate=rx_data.Heart_rate;

			
			
  }
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
    {	
			
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
			
        txflag = 1;
    }
}

void TIM4_IRQHandler(void)   //TIM3�ж�
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
    {	
		
       
       TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
			
			adcx2=Get_Adc_Average(ADC_Channel_1,10);
		
			temp2=(float)adcx2*(3.3/4096);
			sprintf(display,"%d %.1f %.2lf %d",rx_data.Heart_rate,tem+0.05f, temp2,mpu_flag);
						//�ɼ�������ֵ
			if(temp2>=0.75){
						BEEP=1;
				}
			
    }
}