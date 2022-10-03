#include "DHT11.h"
#include "OLED.h" 
#include "Serial.h"               

GPIO_InitTypeDef GPIO_InitStructure;	//后面会改变输入输出状态
                                      //结构体声明在最开头
static void GPIO_SETOUT(void);
static void GPIO_SETIN(void);
static u8 DHT11_Check(void);


/**********************************************
函数名：static void DHT11_Rst(void)
参数说明：无
返回值：无
函数作用：主机发送开始信号
***********************************************/
//这是它的物理工作原理，根据原理拉高或拉低它的引脚来唤醒dht11
static void DHT11_Rst(void)     
{                 
	   GPIO_SETOUT();											//配置成输出模式
     GPIO_ResetBits(DHT11_IO,DHT11_PIN); //拉低数据线
     Delay_ms(20);    										//拉低至少18ms
     GPIO_SetBits(DHT11_IO,DHT11_PIN); 	//拉高数据线 
	   Delay_us(30);     									//主机拉高20~40us
	   GPIO_ResetBits(DHT11_IO,DHT11_PIN);
}


/**********************************************
函数名：u8 DHT11_Init(void)
参数说明：无
返回值：u8 ,返回1代表初始化成功，0则失败
函数作用：配置IO口，并发送开始信号
***********************************************/
u8 DHT11_Init(void){
	
	//IO口初始化配置
	
	RCC_APB2PeriphClockCmd(DHT11_APB2PeriphRCC,ENABLE);//换IO口需要修改,时钟设置
	
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;          //调用引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出，如果需要考虑到IC的电流驱动能力时要接上拉电阻（5K）
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;  //speed 可随意
	GPIO_Init(DHT11_IO,&GPIO_InitStructure);
	
	DHT11_Rst();//发送开始信号
	
	return DHT11_Check();//检测DHT11的响应
}


/**********************************************
函数名：static void GPIO_SETOUT(void)
参数说明：无
返回值：无
函数作用：配置IO口为推挽输出模式
***********************************************/
static void GPIO_SETOUT(void)
{
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出，如果需要考虑到IC的电流驱动能力时要接上拉电阻（5K）
	GPIO_Init(DHT11_IO,&GPIO_InitStructure);
	
}


/**********************************************
函数名：static void GPIO_SETIN(void)
参数说明：无
返回值：无
函数作用：配置IO口为浮空输入模式
***********************************************/
static void GPIO_SETIN(void)
{
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入模式
	GPIO_Init(DHT11_IO,&GPIO_InitStructure);
}


/**********************************************
函数名：static u8 DHT11_Check(void)
参数说明：无
返回值：检测到回应-->返回1，否则0
函数作用：检测DHT11的响应信号
***********************************************/
static u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	GPIO_SETIN();			//设置为输入模式	
	
  while (!GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN) && retry<100)//DHT11会拉低40~50us
	{
		retry++;
		Delay_us(1);
	}
	if(retry >= 100)	//超时未响应/未收到开始信号，退出检测
		return 0;
	else 
		retry = 0;
  while (GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN) && retry<100)//DHT11拉低后会再次拉高40~50us
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=100)		//超时，DHT11工作出错，退出检测
		return 0;
	return 1;					//设备正常响应，可以正常工作
}


/**********************************************
函数名：static u8 DHT11_Read_Bit(void)
参数说明：无
返回值：返回从DHT11上读取的一个Bit数据
函数作用：从DHT11上读取一个Bit数据
***********************************************/
static u8 DHT11_Read_Bit(void)
{
 	u8 retry = 0;
	//DHT11的Bit开始信号为12-14us低电平
	while(GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN) && retry<100)//等待变为低电平(等待Bit开始信号)
	{
		retry++;
		Delay_us(1);
	}
	retry = 0;
	while(!GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN) && retry<100)//等待变高电平（代表数据开始传输）
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(30);//等待30us
	//0信号为26-28us，1信号则为116-118us,所以说超过30us去读取引脚状态就可以知道传输的值了
	if(GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN)) return 1;
	else return 0;		   
}


/***********************************************************************
函数名：static u8 DHT11_Read_Byte(void)
参数说明：无
返回值：返回从DHT11上读取的一个byte数据
函数作用：从DHT11上读取一个byte数据
************************************************************************/
static u8 DHT11_Read_Byte(void)    
{        
  u8 i,dat;
  dat=0;
	
	for (i=0;i<8;i++) 
	{
   	dat<<=1; 
	  dat|=DHT11_Read_Bit();
  }	
	
  return dat;
}


/**************************************************************************
函数名：u8 DHT11_Read_Data(u8 *temp,u8 *humi)
参数说明：temp:用于存放温度值(范围:0~50°)，humi:用于存放湿度值(范围:20%~90%)
返回值：1：成功读取数据，0：读取数据出错
函数作用：从DHT11上读取温湿度数据
***************************************************************************/
u8 DHT11_Read_Data(u8 *temp,u8 *tempL,u8 *humi,u8 *humiL)
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==1)	//设备响应正常
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])//进行校验
		{
			*humi=buf[0];
			*humiL=buf[1];//湿度小数位
			*temp=buf[2]-3;/*与实际温度修正*/
			*tempL=buf[3];//温度小数位
		  
		   Serial_SendString("湿度：");
	       Serial_SendByte(buf[0]/10+0x30);
		   Serial_SendByte(buf[0]%10+0x30);
		   Serial_SendByte('.');
		   Serial_SendByte(buf[1]/10+0x30);
		   Serial_SendByte(buf[1]%10+0x30);
		   Serial_SendByte('\r\n');
		   Serial_SendString("温度：");
	       Serial_SendByte(buf[2]/10+0x30);
		   Serial_SendByte(buf[2]%10+0x30);
		   Serial_SendByte('.');
		   Serial_SendByte(buf[3]/10+0x30);
		   Serial_SendByte(buf[3]%10+0x30);
		}
	}else return 0;		//设备未成功响应，返回0
	return 1;					//读取数据成功返回1
}
void DHT11_show(){
  OLED_ShowString(1,4,"DHT11_TEST");
	/*显示温度*/
	OLED_ShowString(2,3,"temp:");
	OLED_ShowString(2,10,".");
	OLED_ShowString(2,14,"C");
  /*显示湿度*/
	OLED_ShowString(3,3,"humi:");
	OLED_ShowString(3,10,".");
	OLED_ShowString(3,14,"%");
	/*数据显示位*/
//	OLED_ShowNum(2,8,temp,2);
//	OLED_ShowNum(3,8,humi,2);
//  OLED_ShowNum(3,11,humiL,2);
//  OLED_ShowNum(2,11,tempL,2);

}
