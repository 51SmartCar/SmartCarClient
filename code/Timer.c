#include "Timer.h"
#include "Public.h"

sbit Servo    =  P4 ^ 5;             	//���ת����� ��ʱ��0��ʱ�������
sbit MOTORPWM =  P1 ^ 4;           		// ���Ƶ��PWM ת��



void InitMoter(void){

		MOTORPWM = 0;
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


void Timer0_Update(unsigned int   us)	 	
{	
		unsigned int   valu;
		valu = 0xffff-us;  
		TH0  = valu>>8;   	
		TL0  = valu;
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


void Timer1_Init(void)		//5����@11.0592MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x00;		//���ö�ʱ��ֵ
	TH1 = 0x28;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	 ET1 = 1;                        //ʹ�ܶ�ʱ��0�ж�
    EA = 1;
}



void Timer1_Update(uint32 us)
{	

		uint32 valu;
		TR1 = 0;	//�ж�
		valu  = 0xffff-us;  
		TH1   = valu>>8;   	
		TL1   = valu;
		TR1 = 1;	//�����ж�
}


//��ʱ��1�жϷ������
void Timer1_interrupt (void) interrupt 3 using 1    //����Ϊ10ms
{
		static unsigned char   ss = 1;

		switch(ss)
		{
				case 1:
				{
					MOTORPWM =1;
					Timer1_Update(MOTORDUTY);
				}  break;
				case 2:
				{
					MOTORPWM = 0;
					Timer1_Update(0xD7FF - MOTORDUTY);
				}  break;
				case 3:
				{
					Timer1_Update(0xD7FF);
					ss=0;
				}  break;

				default:break;
		}
		ss++;
}
