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

#include "DeviceAction.h"
#include "Delay.h"
#include "Timer.h"
#include "Public.h"



#define FOSC 11059200L       					   //ϵͳƵ��
#define BAUD 115200            					 //���ڲ�����
	
#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3

#define S2_S0 0x01              //P_SW2.0


/*ע���ڽ�������ת�л���ʱ�������ɲ��0.1S�����ٷ�ת�������п���������������PWMΪ100%ʱ�����Ҫ�л�������򣬱�����ɲ��0.1S�����ٸ���ת�źš�*/

bit	B_TX1_Busy;	//����æ��־

bit MOTORRUNING = 1;
bit Engine_Status = 0;//���濪��״̬
bit Auto_Driver = 0;//�Զ���ʻ״��


uint8 DATA_LENGTH = 7;
uint8 CURRENT_LENGTH=0;

uint8 DATA_GET[]=  { 0x7E, 0x00,     0,  0,      0,      0,       0x7E};


void SendData(char *s);
void UART_R(void);                               //��������
void ResponseData(unsigned char *RES_DATA);
void SendAckData(unsigned char *RES_DATA);
void UART2_Init(void);
void Device_Init(void);
void VehicleDiagnosis(unsigned int distance, unsigned char level);
void VehicleAutoDriver(void);

 //����ͷ   �׸�  ����

void main()
{
    Device_Init();
    UART2_Init();
		Timer0_Init();
		Timer1_Init();
		//Timer4_Init();//��ʱ�����ڼ��㳬����
	  WDT_CONTR = 0x06;       //���Ź���ʱ�����ʱ����㹫ʽ: (12 * 32768 * PS) / FOSC (��)
                            //���ÿ��Ź���ʱ����Ƶ��Ϊ32,���ʱ������:
                            //11.0592M : 1.14s
                            //18.432M  : 0.68s
                            //20M      : 0.63s
    WDT_CONTR |= 0x20;      //�������Ź�

    while(1) {
			
			WDT_CONTR |= 0x10;  //ι������
			
//			GetDistance();//���ڼ��㳬����
//			
//			VehicleDiagnosis(Num_Distance, Motor_Level);
//			
//			VehicleAutoDriver();
			
		};
}


//С���Զ���ʻ
void VehicleAutoDriver(void){
	
	if(!Auto_Driver){
			return;
	}
	
	
	
	
	
	

}


///�����ƶ�
void VehicleDiagnosis(unsigned int distance, unsigned char level){
	
	if( 1 == Motor_CurrentStatus() ){

			if( distance <= (20 + 5*(float)level) ){
				Motor_Actions_Status(0,0);
				Motor_Level = 1;
			}
	}
	
}


void Device_Init(void) {
	
		P0M1 = 0;	P0M0 = 0;	//����Ϊ׼˫���
		P1M1 = 0;	P1M0 = 0;	//����Ϊ׼˫���
		P2M1 = 0;	P2M0 = 0;	//����Ϊ׼˫���
		P3M1 = 0;	P3M0 = 0;	//����Ϊ׼˫���
		P4M1 = 0;	P4M0 = 0;	//����Ϊ׼˫���
		P5M1 = 0;	P5M0 = 0;	//����Ϊ׼˫���
		P6M1 = 0;	P6M0 = 0;	//����Ϊ׼˫���
		P7M1 = 0;	P7M0 = 0;	//����Ϊ׼˫���
		
		P1M1 &= ~(0x18);	  //P1.4 P1.3 ����Ϊ�������
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

    S2CON = 0x50;               //8λ�ɱ䲨����
    T2L = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2
    IE2 = 0x01;                 //ʹ�ܴ���2�ж�
    EA = 1;
		B_TX1_Busy = 0;
		IP2 |=0x01;
}


/*----------------------------
UART �жϷ������
-----------------------------*/
void UART2_interrupt() interrupt 8 using 1
{

	 if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //���S2RIλ
         UART_R();
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //���S2TIλ
        B_TX1_Busy = 0;               //��æ��־
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
        S2BUF =s[i];
					while(!(S2CON & S2TI));
						S2CON &= ~S2TI;         //���S2TIλ
    }
}


//����  ���յ�������

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
					case 0x00:{//������
						if( RES_DATA[4]==0x00 && RES_DATA[3]==0x00){
							SendAckData(RES_DATA);
							Led_Actions_NumAndMS(1,80);
						}
						break;
					};
					case 0x01:{//ת��ͽǶ�
						if( 0x00<=RES_DATA[3]<=0x02 && 0x00<=RES_DATA[4]<=0x03){
							SendAckData(RES_DATA);
							
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
									PWMHEIGHT = 0X3565;
								}else if(0x03 <= RES_DATA[4] ){
									PWMHEIGHT = 0X3432;
								}
							}else if(RES_DATA[3] == 0x01){//��ת
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
					case 0x02:{//����
						if( RES_DATA[4]==0x02){
							 Buzzer_Actions_Status(1);
							SendAckData(RES_DATA);
						}else if( RES_DATA[4]==0x01){
							Buzzer_Actions_Status(0);
							SendAckData(RES_DATA);
						}
						break;
					};
					case 0x03:{//��
						if( RES_DATA[4]==0x02){
						Led_Actions_Status(1);
							SendAckData(RES_DATA);
						}else if( RES_DATA[4]==0x01){
							Led_Actions_Status(0);
							SendAckData(RES_DATA);
						}
						break;
					};
					case 0x04:{//���������
						if( 0x00<=RES_DATA[3]<=0x02 && 0x00<=RES_DATA[4]<=0x03 && MOTORRUNING == 1){
							
							  SendAckData(RES_DATA);
							
								if(RES_DATA[3] == 0x00 || RES_DATA[4] == 0x00 ){//ֹͣ
											
											Motor_Actions_Status(0,0);
	/*ע���ڽ�������ת�л���ʱ�������ɲ��0.1S�����ٷ�ת�������п�������������
	��PWMΪ100%ʱ�����Ҫ�л�������򣬱�����ɲ��0.1S�����ٸ���ת�źš�*/

											MOTORRUNING = 0;
											DELAY_MS(120);
											MOTORRUNING = 1;
											
								}else if(RES_DATA[3] == 0x02){//ǰ��
											
											Motor_Actions_Status(1,0);
													
										if(RES_DATA[4] == 0x01){
														Motor_Level = 1;
										}else if(RES_DATA[4] == 0x02){
														Motor_Level = 2;
										}else if(0x03 <= RES_DATA[4] ){
														Motor_Level = 3;
										}
										
							}else if(RES_DATA[3] == 0x01){//����
									  
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
					case 0x05:{//Ѱ��
						if( RES_DATA[4]==0x02){
							SendAckData(RES_DATA);
							LedAndBuzzer_Actions_NumAndMS(3,200);
						}
						break;
					};
					case 0x06:{//����   TODOLED��ʾ����״̬
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
					case 0x07:{//������   
						if( RES_DATA[4]==0x02){
							RES_DATA[4] = Num_Distance &0xff;//��8λ
							RES_DATA[3] = Num_Distance >>8; //��8λ
							SendAckData(RES_DATA);
							Led_Actions_NumAndMS(1,10);
						}
						break;
					};
					case 0x08:{//������ʻ   
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
