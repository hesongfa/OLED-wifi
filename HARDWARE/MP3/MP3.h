#ifndef __MP3_H
#define __MP3_H

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
//#include "common.h"
#include "delay.h"


void MP3_Init(void);
void MP3_Star(u32 Number);
u8 MP3_State(void);

#endif  

