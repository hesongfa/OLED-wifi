#include "SysTick.h"

SYSTICKBIT SysTickBit;  //系统公共定时器状态

int SysTickTime = 0;

/**********************************
***********************************
公共系统定时器初始化     作者：陈开放
***********************************
**********************************/
void SysTick_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_DeInit(TIM2);
	TIM_TimeBaseStruct.TIM_Period=40;//初值
	TIM_TimeBaseStruct.TIM_Prescaler=35999;//预分频
	TIM_TimeBaseStruct.TIM_ClockDivision=0;
	TIM_TimeBaseStruct.TIM_CounterMode=TIM_CounterMode_Up;//向上

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStruct); 
	TIM_ClearFlag(TIM2,TIM_IT_Update);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
	
  *(u16*)(&SysTickBit)=0;
}

/**********************************
***********************************
公共系统定时器跳变沿     作者：陈开放
***********************************
**********************************/
u8 SysTick_JumpEdge(u8* front, u8* now)
{
  if(*front != *now)//跳变沿
	{
		*front=*now;
		return 1;
	}
	else
	{
	  return 0;
	}
}

/**********************************
***********************************
公共系统定时器1中断     作者：陈开放
***********************************
**********************************/
void TIM2_IRQHandler(void)
{
  TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
	SysTickTime++;
	if(SysTickTime > 60000)
	{
	  SysTickTime = 0;
	}
	
	SysTickBit.Time20ms++;
	SysTickBit.Time20ms %= 2;//50ms

	if(SysTickTime%5 == 0)//100ms
	{
		SysTickBit.Time100ms++;
		SysTickBit.Time100ms %= 2;
	}
	if(SysTickTime%25 == 0)//500ms
	{
		SysTickBit.Time500ms++;
		SysTickBit.Time500ms %= 2;
	}
	if(SysTickTime%50 == 0)//1s
	{
		SysTickBit.Time1s++;
		SysTickBit.Time1s %= 2;	
	}
	if(SysTickTime%100 == 0)//2s
	{
		SysTickBit.Time2s++;
		SysTickBit.Time2s %= 2;
	}		
	if(SysTickTime%250 == 0)//5s
	{
		SysTickBit.Time5s++;
		SysTickBit.Time5s %= 2;		
	}
	if(SysTickTime%500 == 0)//10s
	{
		SysTickBit.Time10s++;
		SysTickBit.Time10s %= 2;	
	}
	if(SysTickTime%3000 == 0)//60s
	{
		SysTickBit.Time60s++;
		SysTickBit.Time60s %= 2;
	}
	if(SysTickTime%15000 == 0)//5m
	{
		SysTickBit.Time5m++;
		SysTickBit.Time5m %= 2;	
	}
	if(SysTickTime%30000 == 0)//10m
	{
		SysTickBit.Time10m++;
		SysTickBit.Time10m %= 2;	
	}	
}

