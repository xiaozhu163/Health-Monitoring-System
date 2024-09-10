#ifndef __hmi_usart2_h
#define __hmi_usart2_h

#include "sys.h"

#pragma anon_unions
#pragma pack(push)
#pragma pack(1)

typedef struct{
	int32_t ecg;
	u8 Heart_rate;
}rx_data_t;

#pragma pack(pop)
void usart2_init(void);

extern rx_data_t rx_data;
extern s32 ecg_buf[128];

#endif

