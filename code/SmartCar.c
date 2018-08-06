/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC15F4K60S4 ϵ�� ��ʱ��1��������1�Ĳ����ʷ���������------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966-------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
/* ���Ҫ��������Ӧ�ô˴���,����������ע��ʹ����STC�����ϼ�����        */
/*---------------------------------------------------------------------*/

//�����ر�˵��,����Ƶ��һ��Ϊ11.0592MHz


#include "STC15W4K58S4.h"

#include "intrins.h"
#include <string.h>  // �ַ�������ͷ�ļ�

#define uint8   unsigned char  
#define uint32    unsigned int   
#define uint16    unsigned short int
#define MAIN_Fosc		11059200UL	//������ʱ��
#define T1MS		(65536-MAIN_Fosc/1000) //1MS


sbit Servo  = P1^4;             //��� ��ʱ��0��ʱ�������
sbit LED =    P5 ^ 5;           // LED��
sbit LOUND =  P5 ^ 4;           // ������
sbit MOTORIN1 =  P1 ^ 6;           // ���Ƶ������
sbit MOTORIN2 =  P1 ^ 7;           // ���Ƶ������
/*ע���ڽ�������ת�л���ʱ�������ɲ��0.1S�����ٷ�ת�������п�������������
��PWMΪ100%ʱ�����Ҫ�л�������򣬱�����ɲ��0.1S�����ٸ���ת�źš�*/
sbit MOTORPWM =  P1 ^ 5;           // ���Ƶ��PWM ת��

bit busy;
bit MOTORRUNING = 1;

typedef unsigned char BYTE;
typedef unsigned int WORD;


BYTE DATA_LENGTH = 7;
BYTE CURRENT_LENGTH=0;

BYTE DATA_GET[]=  { 0x7E, 0x00,     0,  0,      0,      0,       0x7E};

/*

//1843

//1.0ms 0X2B32 ����45�Ƚ� 3��
//1. ms 0X3265 ����30�Ƚ� 2�� 
//1. ms 0X3998 ����15�Ƚ� 1��
//1.5ms 0X40CC ����
//1. ms 0X47FE ����15�Ƚ� 1��
//1. ms 0X4F31 ����30�Ƚ� 2�� 
//2.0ms 0X5665 ����45�Ƚ� 3��

*/
uint32 PWMHEIGHT = 0X40CC;

uint32 MOTORDUTY = 0X40CC;


#define FOSC 11059200L       					   //ϵͳƵ��
#define BAUD 115200            					 //���ڲ�����
	
#define S1_S0 0x40              					//P_SW1.6
#define S1_S1 0x80              					//P_SW1.7
	
 
void SendString(char *s);
void SendDatas(char *s);

void SendData(char *s);
void UART_TC (unsigned char *str);
void UART_T (unsigned char UART_data); 			//���崮�ڷ������ݱ���
void UART_R(void);                               //��������
void DELAY_MS(uint8 ms);
void USART_Init(void);
void Device_Init(void);
void ResponseData(unsigned char *RES_DATA);
char CheckData(unsigned char *CHECK_DATA);
void sendAckData(unsigned char *RES_DATA);

void Timer0_Init(void);
void Timer0_Update(uint32 us);

void Timer2_Init(void);
void Timer2_Update(uint32 us);


void main()
{
		P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;
	
    Device_Init();
    USART_Init();
		
		Timer0_Init();

    Timer2_Init();


	  WDT_CONTR = 0x06;       //���Ź���ʱ�����ʱ����㹫ʽ: (12 * 32768 * PS) / FOSC (��)
                            //���ÿ��Ź���ʱ����Ƶ��Ϊ32,���ʱ������:
                            //11.0592M : 1.14s
                            //18.432M  : 0.68s
                            //20M      : 0.63s
    WDT_CONTR |= 0x20;      //�������Ź�

    while(1) {
			WDT_CONTR |= 0x10;  //ι������
		};
}


void Timer0_Init(void)		//@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
  TL0 = 0x33;			//���ö�ʱ��ֵ
	TH0 = 0xBF;			//���ö�ʱ��ֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;        //ʹ�ܶ�ʱ��0�ж�
  EA = 1;
}


void Timer0_Update(uint32 us)	 	
{	
	uint32 valu;
	valu=0xffff-us;  
  TH0=valu>>8;   	
  TL0=valu;
  TR0  = 1;				//T0��ʼ����
}



//��ʱ��0�жϷ������
void Timer0_interrupt() interrupt 1
{
	static unsigned char   i=1;

	switch(i)	  
	{
			case 1:
			{
				Servo = 1;
				
				Timer0_Update(PWMHEIGHT);
			}  break;
			case 2:
			{
			 	Servo=0;     //	pwm1��� 
				
				Timer0_Update(0x6BFF - PWMHEIGHT);
			}  break;
			case 3:
			{
				Timer0_Update(0x6BFF);
			}  break;
			case 4:
			{
				Timer0_Update(0x6BFF);
			}  break;
			case 5:
			{
				Timer0_Update(0x6BFF);
			}  break;
			case 6:
			{
				Timer0_Update(0x6BFF);
			}  break;
			case 7:
			{
				Timer0_Update(0x6BFF);
			}  break;
			case 8:
			{
				Timer0_Update(0x6BFF);
			}  break;
			case 9:
			{
				Timer0_Update(0x6BFF);
        i=0;
			}  break;

			default:break;
	}
	i++;
    
}


void Timer2_Init(void)		//@11.0592MHz  
{
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0xEC;		//���ö�ʱ��ֵ
	T2H = 0xEF;		//���ö�ʱ��ֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
	IE2  |=  (1<<2);	//�����ж�

}



void Timer2_Update(uint32 us)	 	
{	

	uint32 valu;
	IE2  |=  (0<<2);	//�����ж�

	valu=0xffff-us;  
  T2H=valu>>8;   	
  T2L=valu;
	IE2  |=  (1<<2);	//�����ж�
}



//��ʱ��2�жϷ������
void timer2_interrupt (void) interrupt 12
{
	static unsigned char   ss = 1;

	switch(ss)
	{
			case 1:
			{
				MOTORPWM =1;
				
				Timer2_Update(MOTORDUTY);
			}  break;
			case 2:
			{
				MOTORPWM = 0;
				
				Timer2_Update(0x6BFF - MOTORDUTY);
			}  break;
			case 3:
			{
				Timer2_Update(0x6BFF);
			}  break;
			case 4:
			{
				Timer2_Update(0x6BFF);
			}  break;
			case 5:
			{
				Timer2_Update(0x6BFF);
			}  break;
			case 6:
			{
				Timer2_Update(0x6BFF);
			}  break;
			case 7:
			{
				Timer2_Update(0x6BFF);
			}  break;
			case 8:
			{
				Timer2_Update(0x6BFF);
			}  break;
			case 9:
			{
				Timer2_Update(0x6BFF);

        ss=0;
			}  break;

			default:break;
	}
	ss++;
    
}



void Device_Init() {

    LED = 0;
    LOUND = 0;
			
		MOTORIN1 = 0;
		MOTORIN2 = 0 ;
		MOTORPWM = 0;
		MOTORRUNING = 1;
		
}


void USART_Init()
{

//   ACC = P_SW1;
//    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
//    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)

    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=1 S1_S1=0
    ACC |= S1_S0;               //(P3.6/RxD_2, P3.7/TxD_2)
    P_SW1 = ACC;
    SCON = 0x50;                //8λ�ɱ䲨����

//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
//  ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
//  P_SW1 = ACC;


    AUXR = 0x40;                //��ʱ��1Ϊ1Tģʽ
    TMOD = 0x00;                //��ʱ��1Ϊģʽ0(16λ�Զ�����)
    TL1 = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //��ʱ��1��ʼ����
    ES = 1;                     //ʹ�ܴ����ж�
    EA = 1;

}

/*----------------------------
UART �жϷ������
-----------------------------*/
void Uart_interrupt() interrupt 4 using 1
{
    if (RI)
    {
        while(!RI);
        RI=0;
        UART_R();
        busy = 0;

    }
    if (TI)
    {
        while(!TI);
        TI = 0;                 //���TIλ
        busy = 0;               //��æ��־
    }
}


/*----------------------------
���ʹ�������
----------------------------*/

void  SendData(char *s)
{

    unsigned int i=0;

    for(i=0; i<DATA_LENGTH; i++)
    {
        SBUF=s[i];
				while(!TI);		//��鷢���жϱ�־λ
				TI = 0;	
    }
}

void UART_T (unsigned char UART_data) { //���崮�ڷ������ݱ���
    SBUF = UART_data;	//�����յ����ݷ��ͻ�ȥ
    while(!TI);		//��鷢���жϱ�־λ
    TI = 0;			//����жϱ�־λΪ0��������㣩
}


void UART_TC (unsigned char *str) {
    while(*str != '\0') {
        UART_T(*str);
        *str++;
    }
    *str = 0;
}


//����  ���յ�������

void UART_R()
{
    DATA_GET[CURRENT_LENGTH]=SBUF;
    CURRENT_LENGTH++;
    if(CURRENT_LENGTH==DATA_LENGTH)
    {
        CURRENT_LENGTH=0;
        ResponseData(DATA_GET);
    }
}



char CheckData(unsigned char *CHECK_DATA) {

    unsigned char  CHECKSUM = CHECK_DATA[1]+CHECK_DATA[2]+CHECK_DATA[3]+CHECK_DATA[4]-0x01;

    return CHECKSUM;

}


void ResponseData(unsigned char *RES_DATA) {
	if(CheckData(RES_DATA) == RES_DATA[5]) {
			switch(RES_DATA[1]){
				case 0x01:{//ת��ͽǶ�
					if( 0x00<=RES_DATA[3]<=0x02 && 0x00<=RES_DATA[4]<=0x03){
            sendAckData(RES_DATA);
						
						//1.0ms 0X2B32 ����45�Ƚ� 3��
						//1. ms 0X3265 ����30�Ƚ� 2�� 
						//1. ms 0X3998 ����15�Ƚ� 1��
						//1.5ms 0X40CC ����
						//1. ms 0X47FE ����15�Ƚ� 1��
						//1. ms 0X4F31 ����30�Ƚ� 2�� 
						//2.0ms 0X5665 ����45�Ƚ� 3��
						
						if(RES_DATA[3] == 0x00 || RES_DATA[4] == 0x00 ){//����
								PWMHEIGHT = 0X40CC;
						  }else if(RES_DATA[3] == 0x02){//��ת
							if(RES_DATA[4] == 0x01){
								PWMHEIGHT = 0X3998;
						  }else if(RES_DATA[4] == 0x02){
								PWMHEIGHT = 0X3265;
						  }else if(0x03 <= RES_DATA[4] ){
								PWMHEIGHT = 0X2B32;
						  }
						}else if(RES_DATA[3] == 0x01){//��ת
							if(RES_DATA[4] == 0x01){
								PWMHEIGHT = 0X47FE;
						  }else if(RES_DATA[4] == 0x02){
								PWMHEIGHT = 0X4F31;
						  }else if(0x03 <= RES_DATA[4] ){
								PWMHEIGHT = 0X5665;
						  }
						}
						
						
					}
					break;
			  };
				case 0x02:{//����
					if( RES_DATA[4]==0x02){
						 LOUND = 1;
            sendAckData(RES_DATA);
					}else if( RES_DATA[4]==0x01){
						LOUND = 0;
            sendAckData(RES_DATA);
					}
					break;
			  };
				case 0x03:{//��
					if( RES_DATA[4]==0x02){
						LED = 1;
            sendAckData(RES_DATA);
					}else if( RES_DATA[4]==0x01){
						LED = 0;
            sendAckData(RES_DATA);
					}
					break;
			  };
				case 0x04:{//���������
					if( 0x00<=RES_DATA[3]<=0x02 && 0x00<=RES_DATA[4]<=0x03 && MOTORRUNING == 1){
            sendAckData(RES_DATA);
						
						
							if(RES_DATA[3] == 0x00 || RES_DATA[4] == 0x00 ){//ֹͣ
										MOTORIN1=0;
										MOTORIN2=0;
/*ע���ڽ�������ת�л���ʱ�������ɲ��0.1S�����ٷ�ת�������п�������������
��PWMΪ100%ʱ�����Ҫ�л�������򣬱�����ɲ��0.1S�����ٸ���ת�źš�*/

										MOTORRUNING = 0;
					        	DELAY_MS(200);
										MOTORRUNING = 1;
										
						  }else if(RES_DATA[3] == 0x02){//ǰ��
												MOTORIN1=1;
										    MOTORIN2=0;	
												
												
									if(RES_DATA[4] == 0x01){
													MOTORDUTY = 0X0115;
									}else if(RES_DATA[4] == 0x02){
													MOTORDUTY = 0X1033;
									}else if(0x03 <= RES_DATA[4] ){
													MOTORDUTY = 0X5663;
									}
									
						}else if(RES_DATA[3] == 0x01){//����
								MOTORIN1=0;
								MOTORIN2=1;	
												
									if(RES_DATA[4] == 0x01){
													MOTORDUTY = 0X0115;
									}else if(RES_DATA[4] == 0x02){
													MOTORDUTY = 0X1033;
									}else if(0x03 <= RES_DATA[4] ){
													MOTORDUTY = 0X5663;
									}
							
						}
						
					}
					break;
			  };
				case 0x05:{//Ѱ��
					if( RES_DATA[4]==0x02){
						sendAckData(RES_DATA);
						LED = 1;
						LOUND = 1;
						DELAY_MS(200);
						LED = 0;
						LOUND = 0;
						DELAY_MS(200);
						LED = 1;
						LOUND = 1;
						DELAY_MS(200);
						LED = 0;
						LOUND = 0;
						DELAY_MS(200);
						LED = 1;
						LOUND = 1;
						DELAY_MS(200);
						LED = 0;
						LOUND = 0;
						DELAY_MS(200);
					}
					break;
			  };
				case 0x06:{//����   TODOLED��ʾ����״̬
					if( RES_DATA[4]==0x02){
						LED = 1;
						LOUND = 1;
						DELAY_MS(200);
						LED = 0;
						LOUND = 0;
            sendAckData(RES_DATA);
					}else if( RES_DATA[4]==0x01){
						LED = 1;
						LOUND = 1;
						DELAY_MS(200);
						LED = 0;
						LOUND = 0;
            sendAckData(RES_DATA);
					}
					break;
			  };
				
				default:
					break;
				
			}
    }

}


void sendAckData(unsigned char *RES_DATA) {


    unsigned char DATA_SEND[]= { 0x7E, 0x00,     0x02,  0x00,    0x00 ,      0x00,       0x7E};

    DATA_SEND[1]= RES_DATA[1];
    DATA_SEND[3]= RES_DATA[3];
    DATA_SEND[4]= RES_DATA[4];
    DATA_SEND[5]= CheckData(DATA_SEND);

    SendData(DATA_SEND);

}


//========================================================================
// ����: void  delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void  DELAY_MS(uint8 ms)
{
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 13000;
		  while(--i)	;
     }while(--ms);
}