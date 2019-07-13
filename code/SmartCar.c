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

#include "DeviceAction.h"
#include "Delay.h"
#include "Timer.h"
#include "Public.h"



#define FOSC 11059200L       					   //系统频率
#define BAUD 115200            					 //串口波特率
	
#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3

#define S2_S0 0x01              //P_SW2.0


/*注：在进行正反转切换的时候最好先刹车0.1S以上再反转，否则有可能损坏驱动器。在PWM为100%时，如果要切换电机方向，必须先刹车0.1S以上再给反转信号。*/

bit	B_TX1_Busy;	//发送忙标志

bit MOTORRUNING = 1;
bit Engine_Status = 0;//引擎开关状态
bit Auto_Driver = 0;//自动驾驶状体


uint8 DATA_LENGTH = 7;
uint8 CURRENT_LENGTH=0;

uint8 DATA_GET[]=  { 0x7E, 0x00,     0,  0,      0,      0,       0x7E};


void SendData(char *s);
void UART_R(void);                               //接受数据
void ResponseData(unsigned char *RES_DATA);
void SendAckData(unsigned char *RES_DATA);
void UART2_Init(void);
void Device_Init(void);
void VehicleDiagnosis(unsigned int distance, unsigned char level);
void VehicleAutoDriver(void);

 //摄像头   白负  黑正

void main()
{
    Device_Init();
    UART2_Init();
		Timer0_Init();
		Timer1_Init();
		//Timer4_Init();//定时器用于计算超声波
	  WDT_CONTR = 0x06;       //看门狗定时器溢出时间计算公式: (12 * 32768 * PS) / FOSC (秒)
                            //设置看门狗定时器分频数为32,溢出时间如下:
                            //11.0592M : 1.14s
                            //18.432M  : 0.68s
                            //20M      : 0.63s
    WDT_CONTR |= 0x20;      //启动看门狗

    while(1) {
			
			WDT_CONTR |= 0x10;  //喂狗程序
			
//			GetDistance();//用于计算超声波
//			
//			VehicleDiagnosis(Num_Distance, Motor_Level);
//			
//			VehicleAutoDriver();
			
		};
}


//小车自动驾驶
void VehicleAutoDriver(void){
	
	if(!Auto_Driver){
			return;
	}
	
	
	
	
	
	

}


///紧急制动
void VehicleDiagnosis(unsigned int distance, unsigned char level){
	
	if( 1 == Motor_CurrentStatus() ){

			if( distance <= (20 + 5*(float)level) ){
				Motor_Actions_Status(0,0);
				Motor_Level = 1;
			}
	}
	
}


void Device_Init(void) {
	
		P0M1 = 0;	P0M0 = 0;	//设置为准双向口
		P1M1 = 0;	P1M0 = 0;	//设置为准双向口
		P2M1 = 0;	P2M0 = 0;	//设置为准双向口
		P3M1 = 0;	P3M0 = 0;	//设置为准双向口
		P4M1 = 0;	P4M0 = 0;	//设置为准双向口
		P5M1 = 0;	P5M0 = 0;	//设置为准双向口
		P6M1 = 0;	P6M0 = 0;	//设置为准双向口
		P7M1 = 0;	P7M0 = 0;	//设置为准双向口
		
		P1M1 &= ~(0x18);	  //P1.4 P1.3 设置为推挽输出
		P1M0 |=  (0x18);
		P4M1 &= ~(0x20);
		P4M0 |=  (0x20);
		
		
		MOTORRUNING = 1;
		Engine_Status = 0;
		
    Buzzer_Actions_Status(0);
	  Led_Actions_Status(0);
	  InitMoter();
		Motor_Actions_Status(0,0);
		
}


void UART2_Init()
{

    P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)

    S2CON = 0x50;               //8位可变波特率
    T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;                //T2为1T模式, 并启动定时器2
    IE2 = 0x01;                 //使能串口2中断
    EA = 1;
		B_TX1_Busy = 0;
		IP2 |=0x01;
}


/*----------------------------
UART 中断服务程序
-----------------------------*/
void UART2_interrupt() interrupt 8 using 1
{

	 if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //清除S2RI位
         UART_R();
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //清除S2TI位
        B_TX1_Busy = 0;               //清忙标志
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
        S2BUF =s[i];
					while(!(S2CON & S2TI));
						S2CON &= ~S2TI;         //清除S2TI位
    }
}


//串口  接收到的数据

void UART_R()
{
    DATA_GET[CURRENT_LENGTH]=S2BUF ;
    CURRENT_LENGTH++;
		
    if(CURRENT_LENGTH==DATA_LENGTH && !B_TX1_Busy)
    {
				if(DATA_GET[0] == 0x7E && DATA_GET[DATA_LENGTH-1] == 0x7E ){
						CURRENT_LENGTH=0;
						B_TX1_Busy = 1;
						ResponseData(DATA_GET);
				}else {
				
				}
       
    }else if(	CURRENT_LENGTH==2 && DATA_GET[0]==0x7E && DATA_GET[1]==0x7E){
			CURRENT_LENGTH = 1;
		}
		
}


void ResponseData(unsigned char *RES_DATA) {

		if(RES_DATA[2]== 0x01 && CheckData(RES_DATA) == RES_DATA[5]) {
				switch(RES_DATA[1]){
					case 0x00:{//心跳包
						if( RES_DATA[4]==0x00 && RES_DATA[3]==0x00){
							SendAckData(RES_DATA);
							Led_Actions_NumAndMS(1,80);
						}
						break;
					};
					case 0x01:{//转弯和角度
						if( 0x00<=RES_DATA[3]<=0x02 && 0x00<=RES_DATA[4]<=0x03){
							SendAckData(RES_DATA);
							
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
									PWMHEIGHT = 0X3565;
								}else if(0x03 <= RES_DATA[4] ){
									PWMHEIGHT = 0X3432;
								}
							}else if(RES_DATA[3] == 0x01){//右转
								if(RES_DATA[4] == 0x01){
									PWMHEIGHT = 0X47FE;
								}else if(RES_DATA[4] == 0x02){
									PWMHEIGHT = 0X4F31;
								}else if(0x03 <= RES_DATA[4] ){
									PWMHEIGHT = 0X5065;
								}
							}
							
							
						}
						break;
					};
					case 0x02:{//喇叭
						if( RES_DATA[4]==0x02){
							 Buzzer_Actions_Status(1);
							SendAckData(RES_DATA);
						}else if( RES_DATA[4]==0x01){
							Buzzer_Actions_Status(0);
							SendAckData(RES_DATA);
						}
						break;
					};
					case 0x03:{//灯
						if( RES_DATA[4]==0x02){
						Led_Actions_Status(1);
							SendAckData(RES_DATA);
						}else if( RES_DATA[4]==0x01){
							Led_Actions_Status(0);
							SendAckData(RES_DATA);
						}
						break;
					};
					case 0x04:{//方向和油门
						if( 0x00<=RES_DATA[3]<=0x02 && 0x00<=RES_DATA[4]<=0x03 && MOTORRUNING == 1){
							
							  SendAckData(RES_DATA);
							
								if(RES_DATA[3] == 0x00 || RES_DATA[4] == 0x00 ){//停止
											
											Motor_Actions_Status(0,0);
	/*注：在进行正反转切换的时候最好先刹车0.1S以上再反转，否则有可能损坏驱动器。
	在PWM为100%时，如果要切换电机方向，必须先刹车0.1S以上再给反转信号。*/

											MOTORRUNING = 0;
											DELAY_MS(120);
											MOTORRUNING = 1;
											
								}else if(RES_DATA[3] == 0x02){//前进
											
											Motor_Actions_Status(1,0);
													
										if(RES_DATA[4] == 0x01){
														Motor_Level = 1;
										}else if(RES_DATA[4] == 0x02){
														Motor_Level = 2;
										}else if(0x03 <= RES_DATA[4] ){
														Motor_Level = 3;
										}
										
							}else if(RES_DATA[3] == 0x01){//后退
									  
										Motor_Actions_Status(0,1);
													
										if(RES_DATA[4] == 0x01){
														Motor_Level = 1;
										}else if(RES_DATA[4] == 0x02){
														Motor_Level = 2;
										}else if(0x03 <= RES_DATA[4] ){
														Motor_Level = 3;
										}
								
							}
							
						}
						break;
					};
					case 0x05:{//寻车
						if( RES_DATA[4]==0x02){
							SendAckData(RES_DATA);
							LedAndBuzzer_Actions_NumAndMS(3,200);
						}
						break;
					};
					case 0x06:{//引擎   TODOLED显示工作状态
						if( RES_DATA[4]==0x02){
							Engine_Status = 1;
							LedAndBuzzer_Actions_NumAndMS(2,100);
							SendAckData(RES_DATA);
						}else if( RES_DATA[4]==0x01){
							Engine_Status = 0;
							LedAndBuzzer_Actions_NumAndMS(2,100);
							SendAckData(RES_DATA);
						}
						break;
					};
					case 0x07:{//超声波   
						if( RES_DATA[4]==0x02){
							RES_DATA[4] = Num_Distance &0xff;//低8位
							RES_DATA[3] = Num_Distance >>8; //高8位
							SendAckData(RES_DATA);
							Led_Actions_NumAndMS(1,10);
						}
						break;
					};
					case 0x08:{//智能行驶   
						if( RES_DATA[4]==0x02){
							Auto_Driver = 1;
							SendAckData(RES_DATA);
							Led_Actions_NumAndMS(1,10);
						}else if( RES_DATA[4]==0x01){
							Auto_Driver = 0;
							SendAckData(RES_DATA);
						}
						break;
					};
					default:
						break;
					
				}
			}
		
		B_TX1_Busy = 0;

}


void SendAckData(unsigned char *RES_DATA) {

    unsigned char DATA_SEND[]= { 0x7E, 0x00,     0x02,  0x00,    0x00 ,      0x00,       0x7E};

    DATA_SEND[1]= RES_DATA[1];
    DATA_SEND[3]= RES_DATA[3];
    DATA_SEND[4]= RES_DATA[4];
    DATA_SEND[5]= CheckData(DATA_SEND);

    SendData(DATA_SEND);

}
