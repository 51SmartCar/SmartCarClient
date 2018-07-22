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

//本示例在Keil开发环境下请选择Intel的8058芯片型号进行编译
//若无特别说明,工作频率一般为11.0592MHz


#include "STC15W4K58S4.h"

#include "intrins.h"
#include <string.h>  // 字符串处理头文件

sbit LED = P3 ^ 2;  // 对应硬件连接
sbit LOUND = P5 ^ 4;  // 对应硬件连接

bit busy;

typedef unsigned char BYTE;
typedef unsigned int WORD;

BYTE DATA_LENGTH = 7;
BYTE CURRENT_LENGTH=0;

BYTE DATA_GET[]=  { 0x7E, 0x00,     0,  0,      0,      0,       0x7E};


#define FOSC 11059200L          //系统频率
#define BAUD 115200             //串口波特率

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7


void SendString(char *s);
void SendDatas(char *s);

void SendData(char *s);
void UART_TC (unsigned char *str);
void UART_T (unsigned char UART_data); //定义串口发送数据变量
void UART_R();//接受数据
void DELAY_MS(unsigned int timeout);		//@11.0592MHz   1ms
void ConnectServer();//连接服务器
void USART_Init();
void Device_Init();
void ResponseData(unsigned char *RES_DATA);
char CheckData(unsigned char *CHECK_DATA);
void sendAckData(unsigned char *RES_DATA);
void ConnectSuccess();

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

		ConnectServer();

		ConnectSuccess();
		
    while(1) {

    };
}


void Device_Init() {

    LED = 0;
    LOUND = 0;
}

void ConnectSuccess(){

	 LOUND = 1;
	 DELAY_MS(200);
		LOUND = 0;
	 DELAY_MS(200);
	  LOUND = 1;
	 DELAY_MS(200);
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

        if(RES_DATA[1]==0x03 && RES_DATA[4]==0x02) {
            LED = 1;
            sendAckData(RES_DATA);
        } else	if(RES_DATA[1]==0x03 && RES_DATA[4]==0x01) {
            LED = 0;
            sendAckData(RES_DATA);
        } else if(RES_DATA[1]==0x02 && RES_DATA[4]==0x02) {
            LOUND = 1;
            sendAckData(RES_DATA);
        } else	if(RES_DATA[1]==0x02 && RES_DATA[4]==0x01) {
            LOUND = 0;
            sendAckData(RES_DATA);
        }
    }

}


void sendAckData(unsigned char *RES_DATA) {


    unsigned char DATA_SEND[]= { 0x7E, 0x00,     0x02,  0x00,    0x00 ,      0x00,       0x7E};

    DATA_SEND[1]= RES_DATA[1];
    DATA_SEND[4]= RES_DATA[4];
    DATA_SEND[5]= CheckData(RES_DATA);

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


void ConnectServer() {


    UART_TC("+++\0"); // 退出透传模式
    DELAY_MS( 1000);

    //UART_TC("AT\r\n\0");	// AT指令测试
    //DELAY_MS(1500);

    //UART_TC("AT+CWSTARTSMART\r\n\0"); // 开始智能配网模式
    //DELAY_MS(1000);
//	LED = 0; // 配网指示灯亮起
//	DELAY_MS( 30000); // 链接成功

    //UART_TC("AT+CWSTOPSMART\r\n\0"); // 结束智能配网模式，释放模块资源(必须)
//	DELAY_MS( 1000);
    //LED = 1; // 配网指示灯熄灭

    UART_TC("AT+CIPMUX=0\r\n\0");  // 设置单连接模式
    DELAY_MS(1500);

    UART_TC("AT+CIPSTART=\"TCP\",\"192.168.0.104\",4001\r\n\0");	// 连接到指定TCP服务器
    DELAY_MS( 3500);

    UART_TC("AT+CIPMODE=1\r\n\0"); // 设置透传模式
    DELAY_MS( 1500);

    UART_TC("AT+SAVETRANSLINK=1,\"192.168.0.104\",4001,\"TCP\"\r\n\0"); // 保存TCP连接到flash，实现上电透传
    DELAY_MS(1500);

    UART_TC("AT+CIPSEND\r\n\0");	 // 进入透传模式
    DELAY_MS( 1000);

    CURRENT_LENGTH=0;
}
