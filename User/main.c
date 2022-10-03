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


/* 定义线程控制块 */
static rt_thread_t led1_thread = RT_NULL;
 
/*
*************************************************************************
*                             函数声明
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
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//	/*Tout（溢出时间）=（ARR+1)(PSC+1)/Tclk*/
	 //TIM3_Int_Init (6999,7199);//定时器
//	
//	OLED_Clear ();
    // DHT11_show();
//	

//	while (1)
//	{ 
//    
//    }
	
	
	
	
	
	
	
	
	
	
	
	
	  led1_thread =                          /* 线程控制块指针 */
    rt_thread_create( "led1",              /* 线程名字 */
                      led1_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (led1_thread != RT_NULL)
        rt_thread_startup(led1_thread);
    else
        return -1;

	
	
}
/*
*************************************************************************
*                             线程定义
*************************************************************************
*/
 
static void led1_thread_entry(void* parameter)
{	
	while (1)
	{
		LED1_ON();
		
		rt_thread_delay(500);   /* 延时500个tick */
	
		LED1_OFF();     
		rt_thread_delay(500);   /* 延时500个tick */		 		
	
	}
}


void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
		OLED_ShowNum(2,8,temp,2);
	    OLED_ShowNum(3,8,humi,2);
    	OLED_ShowNum(3,11,humiL,2);
		OLED_ShowNum(2,11,tempL,2);
		DHT11_Read_Data(&temp,&tempL ,&humi,&humiL);
		if (Serial_RxFlag==1)
		{
			// OLED_ShowString(5,1,"            ");//清除2行的文字
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


