#include "sys.h"
#include "usart.h"	  
 
//	 
// 	  
 
 
//
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
 
}; 
 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
 	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 
	USART_SendData(USART2,(uint8_t)ch);   
	
	return ch;
}
#endif 
 
/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 
    while (!(USART1->SR & USART_FLAG_RXNE));
    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
 
void uart2_init(u32 bound){
	 //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//ʹ��USART2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//ʹ��GPIOAʱ��
	USART_DeInit(USART2);  //��λ����2
	 //USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
 
	 //USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
 
 
	//USART ��ʼ������
 
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
 
	 USART_Init(USART2, &USART_InitStructure); //��ʼ������
	#if EN_USART1_RX		  //���ʹ���˽���  
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	#endif
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
 
}
 
 
void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res;
	#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
	#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART2);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
	 } 
	#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
	#endif
}


void uart_init(u32 bound){
  //GPIO???????
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//???USART1??GPIOA???
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//???????????
  GPIO_Init(GPIOA, &GPIO_InitStructure);//?????GPIOA.9
   
  //USART1_RX	  GPIOA.10?????
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//????????
  GPIO_Init(GPIOA, &GPIO_InitStructure);//?????GPIOA.10  

  //Usart1 NVIC ????
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//????????3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//???????3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ??????
	NVIC_Init(&NVIC_InitStructure);	//?????????????????VIC?????
  
   //USART ?????????

	USART_InitStructure.USART_BaudRate = bound;//?????????
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//????8��??????
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//?????��
	USART_InitStructure.USART_Parity = USART_Parity_No;//?????��??��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//???????????????
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//?????

  USART_Init(USART1, &USART_InitStructure); //?????????1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//????????????��?
  USART_Cmd(USART1, ENABLE);                    //??????1 

}

void USART1_IRQHandler(void)                	//????1?��???????
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//???SYSTEM_SUPPORT_OS??��????????OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //?????��?(????????????????0x0d 0x0a??��)
		{
		Res =USART_ReceiveData(USART1);	//??????????????
		
		if((USART_RX_STA&0x8000)==0)//????��???
			{
			if(USART_RX_STA&0x4000)//???????0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//???????,??????
				else USART_RX_STA|=0x8000;	//????????? 
				}
			else //??????0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//???????????,??????????	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//???SYSTEM_SUPPORT_OS??��????????OS.
	OSIntExit();  											 
#endif
} 
 
 
#endif	
 