//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//����Э���
#include "onenet.h"

//�����豸
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"

//C��
#include <string.h>

#include "YuYin.h"

#include "MP3.h"

#include "PWM.h"

#include "SysTick.h"


#include "oled.h"


u8 Target1=0;//�������ʱ��
u8 Target2=0;
u8 Target3=0;
u8 Target4=0;
u8 Open_JumpEdge=0;//������Ͱ��λʱ��
u8 WakeUp_Flag=0;//���ѱ�־λ


//////////////////////////////�̵�����ʼ��///////////////////////////////

void jidianqi_Init(void)
{
	
	GPIO_InitTypeDef gpio_initstruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	//��GPIOA��GPIOC��ʱ��
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;									//����Ϊ�������ģʽ
	gpio_initstruct.GPIO_Pin = GPIO_Pin_1 ;					//��ʼ��Pin5
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;									//���ص����Ƶ��
	GPIO_Init(GPIOA, &gpio_initstruct);												//��ʼ��GPIOA
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);                                           //��ʼ���رշ���

}

/////////////////////////////////////////////////////////////////////////

/*
************************************************************
*	�������ƣ�	Hardware_Init
*
*	�������ܣ�	Ӳ����ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��ʼ����Ƭ�������Լ�����豸
************************************************************
*/
void Hardware_Init(void)
{
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϿ�������������

		delay_init();									//systick��ʼ��

		Usart2_Init(115200);							//����2������ESP8266��

		YuYin_Init();//��ʼ������ʶ��ģ��

		MP3_Init();//��ʼ��MP3��������ģ��

		SysTick_Init();//��ʼ���๦�ܶ�ʱ��
		PWM_Init();//��ʼ��PWM

		jidianqi_Init();                                //�̵�����ʼ�� 
	
		OLED_Init();
	

}

void Pre_Oled(void)
{

		OLED_Fill(0xFF);
		delay_ms(500);
		OLED_Fill(0x00);
		delay_ms(500);
		
	  OLED_CLS();
		
		ShowSmartTrash();					//��������Ͱ
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
*	�������ƣ�	main
*
*	�������ܣ�	
*
*	��ڲ�����	��
*
*	���ز�����	0
*
*	˵����		
************************************************************
*/
int main(void)
{
	
	unsigned short timeCount = 0;	//���ͼ������
	
	unsigned char *dataPtr = NULL;
	
	Hardware_Init();				//��ʼ����ΧӲ��
	
	ESP8266_Init();					//��ʼ��ESP8266
	
	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);

	Pre_Oled();  //��ʾ��
	
	
	while(1)
	{
		
		if(++timeCount >= 1000)									//���ͼ��10s
		{
			OneNet_SendData();									//��������		
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
	    OneNet_RevPro(dataPtr);
		
		delay_ms(10);
		
		if(YuYin_RX_STA == 1)//�յ�����ָ��
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_1);//���Ƽ̵�����
			
			if(MP3_State() == 0)
			{
		
				if(YuYin_RX_CMD==49 || YuYin_RX_CMD==50)//����
				{
					GPIO_ResetBits(GPIOA, GPIO_Pin_1); //���Ƽ̵�����
					WakeUp_Flag = 1;//���Ѵʱ�־λ��λ
					MP3_Star(5);  //���
					
				}else if(WakeUp_Flag == 1)
				{
					WakeUp_Flag=0;
					if(YuYin_RX_CMD%4 == 1)//��������
					{
						MP3_Star(1);	//��������
						Target1 = 1;
					}
					else if(YuYin_RX_CMD%4 == 2)//��������
					{
						MP3_Star(2); //��������
						Target2 = 1;
					}
					else if(YuYin_RX_CMD%4 == 3)//�к�����
					{
						 MP3_Star(3);	//�к�����
						Target3 = 1;
					}
					else//�ɻ�����
					{
						 MP3_Star(4); //�ɻ�����
						Target4 = 1;
					}
				}
			}
		
			YuYin_RX_STA = 0;
		
		}
		
		
		if(SysTick_JumpEdge(&Open_JumpEdge, &SysTickBit.Time1s)==1)//������Ͱ��λʱ�䵽
		{
		  if(Target1 != 0)//��������Ͱ��
			{
			  Target1++;
				TIM_SetCompare1(TIM4,1250);//��
				if(Target1>6)
				{
					TIM_SetCompare1(TIM4,500);//ʱ�䵽�ر�
				  Target1 = 0;
				}					
			}
			if(Target2 != 0)//��������Ͱ��
			{
				Target2++;
				TIM_SetCompare2(TIM4,800);//��
				if(Target2>6)
				{
					TIM_SetCompare2(TIM4,300);//ʱ�䵽�ر�
					Target2 = 0;
				}	
			}	
			if(Target3 != 0)//�к�����Ͱ��
			{
				Target3++;
				TIM_SetCompare3(TIM4,1200);//��
				if(Target3>6)
				{
					TIM_SetCompare3(TIM4,400);//ʱ�䵽�ر�
					Target3 = 0;
				}	
			}
			if(Target4 != 0)//�ɻ�����Ͱ��
			{
				Target4++;
				TIM_SetCompare4(TIM4,1200);//��
				if(Target4>6)
				{
					TIM_SetCompare4(TIM4,300);//ʱ�䵽�ر�
					Target4 = 0;
				}	
			}
		}
		
		
	}

}
