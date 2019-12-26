//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>

#include "YuYin.h"

#include "MP3.h"

#include "PWM.h"

#include "SysTick.h"


#include "oled.h"


u8 Target1=0;//舵机控制时间
u8 Target2=0;
u8 Target3=0;
u8 Target4=0;
u8 Open_JumpEdge=0;//开垃圾桶单位时间
u8 WakeUp_Flag=0;//唤醒标志位


//////////////////////////////继电器初始化///////////////////////////////

void jidianqi_Init(void)
{
	
	GPIO_InitTypeDef gpio_initstruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	//打开GPIOA和GPIOC的时钟
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;									//设置为推挽输出模式
	gpio_initstruct.GPIO_Pin = GPIO_Pin_1 ;					//初始化Pin5
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;									//承载的最大频率
	GPIO_Init(GPIOA, &gpio_initstruct);												//初始化GPIOA
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);                                           //初始化关闭风扇

}

/////////////////////////////////////////////////////////////////////////

/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

		delay_init();									//systick初始化

		Usart2_Init(115200);							//串口2，驱动ESP8266用

		YuYin_Init();//初始化语音识别模块

		MP3_Init();//初始化MP3语音播报模块

		SysTick_Init();//初始化多功能定时器
		PWM_Init();//初始化PWM

		jidianqi_Init();                                //继电器初始化 
	
		OLED_Init();
	

}

void Pre_Oled(void)
{

		OLED_Fill(0xFF);
		delay_ms(500);
		OLED_Fill(0x00);
		delay_ms(500);
		
	  OLED_CLS();
		
		ShowSmartTrash();					//智能垃圾桶
		ShowTemperature();
		OLED_ShowStr(32,2,((unsigned char *)":"),2);
		Showdu();
		OLED_ShowStr(86,2,((unsigned char *)"C"),2);
		ShowHump(); 
		OLED_ShowStr(32,4,((unsigned char *)":"),2);
		OLED_ShowStr(72,4,((unsigned char *)"%PH"),2);
	

}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
int main(void)
{
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266
	
	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);

	Pre_Oled();  //显示屏
	
	
	while(1)
	{
		
		if(++timeCount >= 1000)									//发送间隔10s
		{
			OneNet_SendData();									//发送数据		
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
	    OneNet_RevPro(dataPtr);
		
		delay_ms(10);
		
		if(YuYin_RX_STA == 1)//收到语音指令
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_1);//控制继电器开
			
			if(MP3_State() == 0)
			{
		
				if(YuYin_RX_CMD==49 || YuYin_RX_CMD==50)//唤醒
				{
					GPIO_ResetBits(GPIOA, GPIO_Pin_1); //控制继电器关
					WakeUp_Flag = 1;//唤醒词标志位置位
					MP3_Star(5);  //你好
					
				}else if(WakeUp_Flag == 1)
				{
					WakeUp_Flag=0;
					if(YuYin_RX_CMD%4 == 1)//厨余垃圾
					{
						MP3_Star(1);	//厨余垃圾
						Target1 = 1;
					}
					else if(YuYin_RX_CMD%4 == 2)//其他垃圾
					{
						MP3_Star(2); //其他垃圾
						Target2 = 1;
					}
					else if(YuYin_RX_CMD%4 == 3)//有害垃圾
					{
						 MP3_Star(3);	//有害垃圾
						Target3 = 1;
					}
					else//可回收物
					{
						 MP3_Star(4); //可回收物
						Target4 = 1;
					}
				}
			}
		
			YuYin_RX_STA = 0;
		
		}
		
		
		if(SysTick_JumpEdge(&Open_JumpEdge, &SysTickBit.Time1s)==1)//开垃圾桶单位时间到
		{
		  if(Target1 != 0)//厨余垃圾桶打开
			{
			  Target1++;
				TIM_SetCompare1(TIM4,1250);//打开
				if(Target1>6)
				{
					TIM_SetCompare1(TIM4,500);//时间到关闭
				  Target1 = 0;
				}					
			}
			if(Target2 != 0)//其他垃圾桶打开
			{
				Target2++;
				TIM_SetCompare2(TIM4,800);//打开
				if(Target2>6)
				{
					TIM_SetCompare2(TIM4,300);//时间到关闭
					Target2 = 0;
				}	
			}	
			if(Target3 != 0)//有害垃圾桶打开
			{
				Target3++;
				TIM_SetCompare3(TIM4,1200);//打开
				if(Target3>6)
				{
					TIM_SetCompare3(TIM4,400);//时间到关闭
					Target3 = 0;
				}	
			}
			if(Target4 != 0)//可回收物桶打开
			{
				Target4++;
				TIM_SetCompare4(TIM4,1200);//打开
				if(Target4>6)
				{
					TIM_SetCompare4(TIM4,300);//时间到关闭
					Target4 = 0;
				}	
			}
		}
		
		
	}

}
