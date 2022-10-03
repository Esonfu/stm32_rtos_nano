 #include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "LED.h"
#include "TIM.h"
#include "Serial.h"
#include "Key.h"

#include "board.h"

#include <string.h>


u8 temp = 0,humi = 0,tempL=0 ,humiL=0;


/* �����߳̿��ƿ� */
static rt_thread_t led1_thread = RT_NULL;
 
/*
*************************************************************************
*                             ��������
*************************************************************************
*/

static void led1_thread_entry(void* parameter);

int main(void)
{
	// Key_Init();
	// Serial_Init();
	// OLED_Init();
    // DHT11_Init();
    LED_Init();
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	/*Tout�����ʱ�䣩=��ARR+1)(PSC+1)/Tclk*/
	 //TIM3_Int_Init (6999,7199);//��ʱ��
//	
//	OLED_Clear ();
    // DHT11_show();
//	

//	while (1)
//	{ 
//    
//    }
	
	
	
	
	
	
	
	
	
	
	
	
	  led1_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "led1",              /* �߳����� */
                      led1_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (led1_thread != RT_NULL)
        rt_thread_startup(led1_thread);
    else
        return -1;

	
	
}
/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
 
static void led1_thread_entry(void* parameter)
{	
	while (1)
	{
		LED1_ON();
		
		rt_thread_delay(500);   /* ��ʱ500��tick */
	
		LED1_OFF();     
		rt_thread_delay(500);   /* ��ʱ500��tick */		 		
	
	}
}


void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		OLED_ShowNum(2,8,temp,2);
	    OLED_ShowNum(3,8,humi,2);
    	OLED_ShowNum(3,11,humiL,2);
		OLED_ShowNum(2,11,tempL,2);
		DHT11_Read_Data(&temp,&tempL ,&humi,&humiL);
		if (Serial_RxFlag==1)
		{
			// OLED_ShowString(5,1,"            ");//���2�е�����
			// OLED_ShowString(5,1,Serial_RxPacket);
			if (strcmp(Serial_RxPacket,"LED_ON")==0)
			{
				LED1_ON();
				Serial_SendString("LED_ON");
				OLED_ShowString(4,1,"                  ");
				OLED_ShowString(4,3,"LED_ON");
			}
			else if (strcmp(Serial_RxPacket,"LED_OFF")==0)
			{
				LED1_OFF();
				Serial_SendString("LED_OFF");
				OLED_ShowString(4,1,"                 ");
				OLED_ShowString(4,3,"LED_OFF");
			}
			else
			{
				Serial_SendString("erro_cmd");
				OLED_ShowString(4,1,"           ");
				OLED_ShowString(4,3,"erro_cmd");
			}
		}
		Serial_RxFlag=0;
		if(temp>28){
		 LED2_ON();
		}
		else {
		 LED2_OFF();
		}
		}
}


