/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC15F4K60S4 系列 定时器1用作串口1的波特率发生器举例------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966-------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/* 如果要在文章中应用此代码,请在文章中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

//若无特别说明,工作频率一般为11.0592MHz


#include "STC15W4K58S4.h"

#include "intrins.h"
#include <string.h>  // 字符串处理头文件

#define uint8   unsigned char  
#define uint32    unsigned int   
#define uint16    unsigned short int
#define MAIN_Fosc		11059200UL	//定义主时钟
#define T1MS		(65536-MAIN_Fosc/1000) //1MS


sbit T0CLKO = P3^5;             //定时器0的时钟输出口
sbit LED =    P3 ^ 2;           // 对应硬件连接
sbit LOUND =  P5 ^ 4;           // 对应硬件连接
sbit MOTORIN1 =  P1 ^ 6;           // 控制电机方向
sbit MOTORIN2 =  P1 ^ 7;           // 控制电机方向
/*注：在进行正反转切换的时候最好先刹车0.1S以上再反转，否则有可能损坏驱动器。
在PWM为100%时，如果要切换电机方向，必须先刹车0.1S以上再给反转信号。*/
sbit MOTORPWM =  P1 ^ 5;           // 控制电机PWM

bit busy;
bit MOTORRUNING = 1;

typedef unsigned char BYTE;
typedef unsigned int WORD;


BYTE DATA_LENGTH = 7;
BYTE CURRENT_LENGTH=0;

BYTE DATA_GET[]=  { 0x7E, 0x00,     0,  0,      0,      0,       0x7E};



//1843

//1.0ms 0X2B32 向左45度角 3挡
//1. ms 0X3265 向左30度角 2挡 
//1. ms 0X3998 向左15度角 1挡
//1.5ms 0X40CC 居中
//1. ms 0X47FE 向右15度角 1挡
//1. ms 0X4F31 向右30度角 2挡 
//2.0ms 0X5665 向右45度角 3挡

uint32 PWMHEIGHT = 0X40CC;


#define FOSC 11059200L       					   //系统频率
#define BAUD 115200            					 //串口波特率
	
#define S1_S0 0x40              					//P_SW1.6
#define S1_S1 0x80              					//P_SW1.7
	

void SendString(char *s);
void SendDatas(char *s);

void SendData(char *s);
void UART_TC (unsigned char *str);
void UART_T (unsigned char UART_data); 			//定义串口发送数据变量
void UART_R();//接受数据
void DELAY_MS(unsigned int timeout);				//@11.0592MHz   1ms
void Delay200ms(void);
void USART_Init();
void Device_Init();
void ResponseData(unsigned char *RES_DATA);
char CheckData(unsigned char *CHECK_DATA);
void sendAckData(unsigned char *RES_DATA);

void Timer0Init(void);
void Timer0(uint32 us);

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
		
		MOTORIN1 = 0;
		MOTORIN2 = 0 ;
		MOTORPWM = 0;
		MOTORRUNING = 1;
		
    Device_Init();
    USART_Init();
		Timer0Init();

    while(1) {};
}


void Timer0Init(void)		//500微秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
  TL0 = 0xCC;			//设置定时初值
	TH0 = 0x40;			//设置定时初值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;        //使能定时器0中断
  EA = 1;
}


void Timer0(uint32 us)	 	
{	
	uint32 valu;
	valu=0xffff-us;  
  TH0=valu>>8;   	
  TL0=valu;
  TR0  = 1;				//T0开始工作
}



//定时器0中断服务程序
void tm0() interrupt 1
{
	static unsigned char   i=1;

	switch(i)	  
	{
			case 1:
			{
				T0CLKO=1;
				MOTORPWM =1;
				
				Timer0(PWMHEIGHT);
			}  break;
			case 2:
			{
			 	T0CLKO=0;     //	pwm1变低 
				MOTORPWM = 0;
				
				Timer0(0x6BFF - PWMHEIGHT);
			}  break;
			case 3:
			{
				Timer0(0x6BFF);
			}  break;
			case 4:
			{
				Timer0(0x6BFF);
			}  break;
			case 5:
			{
				Timer0(0x6BFF);
			}  break;
			case 6:
			{
				Timer0(0x6BFF);
			}  break;
			case 7:
			{
				Timer0(0x6BFF);
			}  break;
			case 8:
			{
				Timer0(0x6BFF);
			}  break;
			case 9:
			{
				Timer0(0x6BFF);
        i=0;
			}  break;

			default:break;
	}
	i++;
    
}

void Device_Init() {

    LED = 0;
    LOUND = 0;
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
    SCON = 0x50;                //8位可变波特率

//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
//  ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
//  P_SW1 = ACC;


    AUXR = 0x40;                //定时器1为1T模式
    TMOD = 0x00;                //定时器1为模式0(16位自动重载)
    TL1 = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //定时器1开始启动
    ES = 1;                     //使能串口中断
    EA = 1;

}

/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart() interrupt 4 using 1
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
        TI = 0;                 //清除TI位
        busy = 0;               //清忙标志
    }
}


/*----------------------------
发送串口数据
----------------------------*/

void  SendData(char *s)
{

    unsigned int i=0;

    for(i=0; i<DATA_LENGTH; i++)
    {
        SBUF=s[i];
				while(!TI);		//检查发送中断标志位
				TI = 0;	
    }
}

void UART_T (unsigned char UART_data) { //定义串口发送数据变量
    SBUF = UART_data;	//将接收的数据发送回去
    while(!TI);		//检查发送中断标志位
    TI = 0;			//令发送中断标志位为0（软件清零）
}


void UART_TC (unsigned char *str) {
    while(*str != '\0') {
        UART_T(*str);
        *str++;
    }
    *str = 0;
}


//串口  接收到的数据

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
				case 0x01:{//转弯和角度
					if( 0x00<=RES_DATA[3]<=0x02 && 0x00<=RES_DATA[4]<=0x03){
            sendAckData(RES_DATA);
						
						//1.0ms 0X2B32 向左45度角 3挡
						//1. ms 0X3265 向左30度角 2挡 
						//1. ms 0X3998 向左15度角 1挡
						//1.5ms 0X40CC 居中
						//1. ms 0X47FE 向右15度角 1挡
						//1. ms 0X4F31 向右30度角 2挡 
						//2.0ms 0X5665 向右45度角 3挡
						
						if(RES_DATA[3] == 0x00 || RES_DATA[4] == 0x00 ){//居中
								PWMHEIGHT = 0X40CC;
						  }else if(RES_DATA[3] == 0x02){//左转
							if(RES_DATA[4] == 0x01){
								PWMHEIGHT = 0X3998;
						  }else if(RES_DATA[4] == 0x02){
								PWMHEIGHT = 0X3265;
						  }else if(0x03 <= RES_DATA[4] ){
								PWMHEIGHT = 0X2B32;
						  }
						}else if(RES_DATA[3] == 0x01){//右转
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
				case 0x02:{//喇叭
					if( RES_DATA[4]==0x02){
						 LOUND = 1;
            sendAckData(RES_DATA);
					}else if( RES_DATA[4]==0x01){
						LOUND = 0;
            sendAckData(RES_DATA);
					}
					break;
			  };
				case 0x03:{//灯
					if( RES_DATA[4]==0x02){
						LED = 1;
            sendAckData(RES_DATA);
					}else if( RES_DATA[4]==0x01){
						LED = 0;
            sendAckData(RES_DATA);
					}
					break;
			  };
				case 0x04:{//方向和油门
					if( 0x00<=RES_DATA[3]<=0x02 && 0x00<=RES_DATA[4]<=0x03 && MOTORRUNING == 1){
            sendAckData(RES_DATA);
						
						
							if(RES_DATA[3] == 0x00 || RES_DATA[4] == 0x00 ){//停止
										MOTORIN1=0;
										MOTORIN2=0;
/*注：在进行正反转切换的时候最好先刹车0.1S以上再反转，否则有可能损坏驱动器。
在PWM为100%时，如果要切换电机方向，必须先刹车0.1S以上再给反转信号。*/

										MOTORRUNING = 0;
										Delay200ms();
										MOTORRUNING = 1;
										
						  }else if(RES_DATA[3] == 0x02){//前进
												MOTORIN1=1;
										    MOTORIN2=0;	
												
									if(RES_DATA[4] == 0x01){
	
									}else if(RES_DATA[4] == 0x02){
									
									}else if(0x03 <= RES_DATA[4] ){
										
									}
						}else if(RES_DATA[3] == 0x01){//后退
								MOTORIN1=0;
								MOTORIN2=1;	
												
							if(RES_DATA[4] == 0x01){
								
						  }else if(RES_DATA[4] == 0x02){
								
						  }else if(0x03 <= RES_DATA[4] ){
								
						  }
						}
						
						
						
					}
					break;
			  };
				case 0x05:{//寻车
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
				case 0x06:{//引擎   TODOLED显示工作状态
					if( RES_DATA[4]==0x02){
						LED = 1;
						LOUND = 1;
						DELAY_MS(1000);
						LED = 0;
						LOUND = 0;
            sendAckData(RES_DATA);
					}else if( RES_DATA[4]==0x01){
						LED = 1;
						LOUND = 1;
						DELAY_MS(1000);
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



void DELAY_1MS() {
    unsigned char i, j;

    _nop_();
    _nop_();
    _nop_();
    i = 11;
    j = 190;
    do
    {
        while (--j);
    } while (--i);


}

void DELAY_MS(unsigned int timeout)		//@11.0592MHz
{
    int t = 0;
    while (t < timeout)
    {
        t++;
        DELAY_1MS();
    }
}

void Delay200ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 9;
	j = 104;
	k = 139;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
