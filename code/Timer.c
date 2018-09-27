#include "Timer.h"
#include "Public.h"

sbit Servo    =  P4 ^ 5;             	//舵机转向控制 定时器0的时钟输出口
sbit MOTORPWM =  P1 ^ 4;           		// 控制电机PWM 转速



void InitMoter(void){

		MOTORPWM = 0;
}


void Timer0_Init(void)		//@11.0592MHz
{
		AUXR |= 0x80;		//定时器时钟1T模式
		TMOD &= 0xF0;		//设置定时器模式
		TMOD |= 0x01;		//设置定时器模式
		TL0 = 0x33;			//设置定时初值
		TH0 = 0xBF;			//设置定时初值
		TF0 = 0;				//清除TF0标志
		TR0 = 1;				//定时器0开始计时
		ET0 = 1;        //使能定时器0中断
		EA = 1;
}


void Timer0_Update(unsigned int   us)	 	
{	
		unsigned int   valu;
		valu = 0xffff-us;  
		TH0  = valu>>8;   	
		TL0  = valu;
		TR0  = 1;				//T0开始工作
}



//定时器0中断服务程序
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
					Servo=0;     //	pwm1变低 
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


void Timer1_Init(void)		//5毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x00;		//设置定时初值
	TH1 = 0x28;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	 ET1 = 1;                        //使能定时器0中断
    EA = 1;
}



void Timer1_Update(uint32 us)
{	

		uint32 valu;
		TR1 = 0;	//中断
		valu  = 0xffff-us;  
		TH1   = valu>>8;   	
		TL1   = valu;
		TR1 = 1;	//允许中断
}


//定时器1中断服务程序
void Timer1_interrupt (void) interrupt 3 using 1    //周期为10ms
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
