#include "sys.h"
#include "delay.h"
#include "math.h"
#include "time.h"
#include "usart.h"
#include "stdio.h"
#include "ads1292r.h"

uint8_t ad_b[9] ;   /* 存储原始数据 */
int32_t ecg_data ;  /* 转换后的i32数据 */
int32_t ecg_data_s[9] = {0};
int32_t ecg_sum;
int32_t ecg;

int32_t ecg_buf[30]= {0};//滑动检测
int32_t ecg_max= 0;
int32_t ecg_min= 0;

u32 tim3_tick = 0;
u32 L_tim3_tick = 0;
u8 Heart_rate;
u8 Heart_buff[10]= {80,80,80,80,80,80,80,80,80,80};

#pragma anon_unions
#pragma pack(push)
#pragma pack(1)

union tx
{
    struct {
        u8 head;
        int32_t ecg;
        u8 Heart_rate;
    }data;
    u8 buf[6];
} tx_data;
#pragma pack(pop)

int32_t i24toi32( uint8_t *p_i32);

u8 start_tx = 0;

int main(void)
{
    u8 i;

    delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    uart_init(256000);

    while(ads1292r_init(5) == 0) {}
    ads1292r_normal_adc_start();
    Time3_Init();
	tx_data.data.head = 0xff;
    while(1)
    {
        ads1292r_get_value(ad_b);
        ecg_data = i24toi32(ad_b+6);  /* 转换原始数据 */
        ecg_sum = 0;
        for(i = 0; i < 8; i++)
        {
            ecg_data_s[i] = ecg_data_s[i+1];
            ecg_sum += ecg_data_s[i];
        }
        ecg_data_s[8] = ecg_data;
        ecg = (ecg_sum + ecg_data) / 9;

        ecg_min = 5000000;
        ecg_max = -5000000;
        for(i=0; i < 29; i++)
        {
            ecg_buf[i]=ecg_buf[i+1];
            ecg_max = ecg_max < ecg_buf[i] ? ecg_buf[i] : ecg_max;
            ecg_min = ecg_min > ecg_buf[i] ? ecg_buf[i] : ecg_min;
        }
        ecg_buf[29]=ecg;
        ecg_max = ecg_max < ecg ? ecg : ecg_max;
        ecg_min = ecg_min > ecg ? ecg : ecg_min;

        if((ecg_max - ecg_min > 30000)&&(tim3_tick > 200))
        {
            if((60000/tim3_tick < 200) && (60000/tim3_tick > 40))
            {
                Heart_buff[0] = Heart_buff[1];
                Heart_buff[1] = Heart_buff[2];
                Heart_buff[2] = Heart_buff[3];
                Heart_buff[3] = Heart_buff[4];
                Heart_buff[4] = Heart_buff[5];
                Heart_buff[5] = Heart_buff[6];
                Heart_buff[6] = Heart_buff[7];
                Heart_buff[7] = Heart_buff[8];
                Heart_buff[8] = Heart_buff[9];
                Heart_buff[9] = 60000/tim3_tick;

                Heart_rate=(Heart_buff[0]+Heart_buff[1]+Heart_buff[2]+Heart_buff[3]+Heart_buff[4]
                            +Heart_buff[5]+Heart_buff[6]+Heart_buff[7]+Heart_buff[8]+Heart_buff[9])/10;
            }

            tim3_tick = 0;
        }
		if(start_tx)//10ms发一次
        {
			start_tx = 0;
			tx_data.data.ecg = ecg;
			tx_data.data.Heart_rate = Heart_rate;
			for ( i = 0; i < 6; i++)
			{
				while((USART1->SR&0X40)==0) ;   /* 循环发送,直到发送完毕 */
				USART1->DR = tx_data.buf[i] ;
			}
//			printf("%d,",ecg);
		}
    }
}
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	static u32 tim3_cnt = 0;
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        tim3_tick ++;
        tim3_cnt ++;
		if(tim3_cnt == 10)
		{
			tim3_cnt = 0;
			start_tx = 1;
		}
    }
}

int32_t i24toi32( uint8_t *p_i32)
{
    int32_t rev = 0 ;

    rev = ( ( (int32_t)p_i32[0]) << 16)
          | ( ( (int32_t) p_i32[1]) << 8) | ( (int32_t) p_i32[2]) ;

    if ( ( p_i32[0] & 0x80) == 0x80)
    {
        rev |= 0xFF000000 ;
    }

    return rev ;
}
