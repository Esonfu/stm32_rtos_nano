#include "DHT11.h"
#include "OLED.h" 
#include "Serial.h"               

GPIO_InitTypeDef GPIO_InitStructure;	//�����ı��������״̬
                                      //�ṹ���������ͷ
static void GPIO_SETOUT(void);
static void GPIO_SETIN(void);
static u8 DHT11_Check(void);


/**********************************************
��������static void DHT11_Rst(void)
����˵������
����ֵ����
�������ã��������Ϳ�ʼ�ź�
***********************************************/
//��������������ԭ������ԭ�����߻�������������������dht11
static void DHT11_Rst(void)     
{                 
	   GPIO_SETOUT();											//���ó����ģʽ
     GPIO_ResetBits(DHT11_IO,DHT11_PIN); //����������
     Delay_ms(20);    										//��������18ms
     GPIO_SetBits(DHT11_IO,DHT11_PIN); 	//���������� 
	   Delay_us(30);     									//��������20~40us
	   GPIO_ResetBits(DHT11_IO,DHT11_PIN);
}


/**********************************************
��������u8 DHT11_Init(void)
����˵������
����ֵ��u8 ,����1�����ʼ���ɹ���0��ʧ��
�������ã�����IO�ڣ������Ϳ�ʼ�ź�
***********************************************/
u8 DHT11_Init(void){
	
	//IO�ڳ�ʼ������
	
	RCC_APB2PeriphClockCmd(DHT11_APB2PeriphRCC,ENABLE);//��IO����Ҫ�޸�,ʱ������
	
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;          //��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //��������������Ҫ���ǵ�IC�ĵ�����������ʱҪ���������裨5K��
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;  //speed ������
	GPIO_Init(DHT11_IO,&GPIO_InitStructure);
	
	DHT11_Rst();//���Ϳ�ʼ�ź�
	
	return DHT11_Check();//���DHT11����Ӧ
}


/**********************************************
��������static void GPIO_SETOUT(void)
����˵������
����ֵ����
�������ã�����IO��Ϊ�������ģʽ
***********************************************/
static void GPIO_SETOUT(void)
{
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //��������������Ҫ���ǵ�IC�ĵ�����������ʱҪ���������裨5K��
	GPIO_Init(DHT11_IO,&GPIO_InitStructure);
	
}


/**********************************************
��������static void GPIO_SETIN(void)
����˵������
����ֵ����
�������ã�����IO��Ϊ��������ģʽ
***********************************************/
static void GPIO_SETIN(void)
{
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������ģʽ
	GPIO_Init(DHT11_IO,&GPIO_InitStructure);
}


/**********************************************
��������static u8 DHT11_Check(void)
����˵������
����ֵ����⵽��Ӧ-->����1������0
�������ã����DHT11����Ӧ�ź�
***********************************************/
static u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	GPIO_SETIN();			//����Ϊ����ģʽ	
	
  while (!GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN) && retry<100)//DHT11������40~50us
	{
		retry++;
		Delay_us(1);
	}
	if(retry >= 100)	//��ʱδ��Ӧ/δ�յ���ʼ�źţ��˳����
		return 0;
	else 
		retry = 0;
  while (GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN) && retry<100)//DHT11���ͺ���ٴ�����40~50us
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=100)		//��ʱ��DHT11���������˳����
		return 0;
	return 1;					//�豸������Ӧ��������������
}


/**********************************************
��������static u8 DHT11_Read_Bit(void)
����˵������
����ֵ�����ش�DHT11�϶�ȡ��һ��Bit����
�������ã���DHT11�϶�ȡһ��Bit����
***********************************************/
static u8 DHT11_Read_Bit(void)
{
 	u8 retry = 0;
	//DHT11��Bit��ʼ�ź�Ϊ12-14us�͵�ƽ
	while(GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN) && retry<100)//�ȴ���Ϊ�͵�ƽ(�ȴ�Bit��ʼ�ź�)
	{
		retry++;
		Delay_us(1);
	}
	retry = 0;
	while(!GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN) && retry<100)//�ȴ���ߵ�ƽ���������ݿ�ʼ���䣩
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(30);//�ȴ�30us
	//0�ź�Ϊ26-28us��1�ź���Ϊ116-118us,����˵����30usȥ��ȡ����״̬�Ϳ���֪�������ֵ��
	if(GPIO_ReadInputDataBit(DHT11_IO,DHT11_PIN)) return 1;
	else return 0;		   
}


/***********************************************************************
��������static u8 DHT11_Read_Byte(void)
����˵������
����ֵ�����ش�DHT11�϶�ȡ��һ��byte����
�������ã���DHT11�϶�ȡһ��byte����
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
��������u8 DHT11_Read_Data(u8 *temp,u8 *humi)
����˵����temp:���ڴ���¶�ֵ(��Χ:0~50��)��humi:���ڴ��ʪ��ֵ(��Χ:20%~90%)
����ֵ��1���ɹ���ȡ���ݣ�0����ȡ���ݳ���
�������ã���DHT11�϶�ȡ��ʪ������
***************************************************************************/
u8 DHT11_Read_Data(u8 *temp,u8 *tempL,u8 *humi,u8 *humiL)
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==1)	//�豸��Ӧ����
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])//����У��
		{
			*humi=buf[0];
			*humiL=buf[1];//ʪ��С��λ
			*temp=buf[2]-3;/*��ʵ���¶�����*/
			*tempL=buf[3];//�¶�С��λ
		  
		   Serial_SendString("ʪ�ȣ�");
	       Serial_SendByte(buf[0]/10+0x30);
		   Serial_SendByte(buf[0]%10+0x30);
		   Serial_SendByte('.');
		   Serial_SendByte(buf[1]/10+0x30);
		   Serial_SendByte(buf[1]%10+0x30);
		   Serial_SendByte('\r\n');
		   Serial_SendString("�¶ȣ�");
	       Serial_SendByte(buf[2]/10+0x30);
		   Serial_SendByte(buf[2]%10+0x30);
		   Serial_SendByte('.');
		   Serial_SendByte(buf[3]/10+0x30);
		   Serial_SendByte(buf[3]%10+0x30);
		}
	}else return 0;		//�豸δ�ɹ���Ӧ������0
	return 1;					//��ȡ���ݳɹ�����1
}
void DHT11_show(){
  OLED_ShowString(1,4,"DHT11_TEST");
	/*��ʾ�¶�*/
	OLED_ShowString(2,3,"temp:");
	OLED_ShowString(2,10,".");
	OLED_ShowString(2,14,"C");
  /*��ʾʪ��*/
	OLED_ShowString(3,3,"humi:");
	OLED_ShowString(3,10,".");
	OLED_ShowString(3,14,"%");
	/*������ʾλ*/
//	OLED_ShowNum(2,8,temp,2);
//	OLED_ShowNum(3,8,humi,2);
//  OLED_ShowNum(3,11,humiL,2);
//  OLED_ShowNum(2,11,tempL,2);

}
