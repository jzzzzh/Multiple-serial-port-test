#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h" 
#include "QDTFT_demo.h"
#include "Lcd_Driver.h"
#include "GUI.h"
int main(void)
{		
	u8 t;
	u8 len;	
	char msg[30];
	u16 times=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart2_init(115200);	 //���ڳ�ʼ��Ϊ115200
	uart_init(115200);
	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init();          //��ʼ���밴�����ӵ�Ӳ���ӿ�
	Lcd_Init();
	LCD_LED_SET;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(0,70,BLUE,GRAY0, "bule test");
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("�����͵���ϢΪ:\r\n");
			Lcd_Clear(GRAY0);
			Gui_DrawFont_GBK16(0,70,BLUE,GRAY0, "�����͵�����Ϊ");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART2, USART_RX_BUF[t]);//�򴮿�2��������
				USART_SendData(USART1, USART_RX_BUF[t]);//�򴮿�1��������
				msg[t]=USART_RX_BUF[t];
 				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET&&USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			msg[len] = '\0';
			Gui_DrawFont_GBK16(10,90,BLUE,GRAY0, msg);
			printf("\r\n");//���뻻��
			USART_RX_STA=0;
		}
		else
		{
			//times++;
			//if(times%5000==0)
			//{
				//printf("\r\n����ʵ��\r\n");
				//printf("wantin\r\n\r\n");
			//}
			//if(times%200==0)printf("����������,�Իس�������\n");  
			//if(times%30==0)LED0=!LED0;//��˸LED,��ʾϵͳ��������.
			delay_ms(10);   
		}
	}	 
}
 