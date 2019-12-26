#ifndef __YuYin_H
#define	__YuYin_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
//#include "common.h"
#include "stdio.h"

extern u8 YuYin_RX_CMD;//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8 YuYin_RX_STA;//接收状态标记	

void YuYin_Init(void);
	
#endif 
