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
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart2_init(115200);	 //串口初始化为115200
	uart_init(115200);
	LED_Init();			     //LED端口初始化
	KEY_Init();          //初始化与按键连接的硬件接口
	Lcd_Init();
	LCD_LED_SET;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(0,70,BLUE,GRAY0, "bule test");
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("您发送的消息为:\r\n");
			Lcd_Clear(GRAY0);
			Gui_DrawFont_GBK16(0,70,BLUE,GRAY0, "您发送的数据为");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART2, USART_RX_BUF[t]);//向串口2发送数据
				USART_SendData(USART1, USART_RX_BUF[t]);//向串口1发送数据
				msg[t]=USART_RX_BUF[t];
 				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET&&USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
			msg[len] = '\0';
			Gui_DrawFont_GBK16(10,90,BLUE,GRAY0, msg);
			printf("\r\n");//插入换行
			USART_RX_STA=0;
		}
		else
		{
			//times++;
			//if(times%5000==0)
			//{
				//printf("\r\n串口实验\r\n");
				//printf("wantin\r\n\r\n");
			//}
			//if(times%200==0)printf("请输入数据,以回车键结束\n");  
			//if(times%30==0)LED0=!LED0;//闪烁LED,提示系统正在运行.
			delay_ms(10);   
		}
	}	 
}
 