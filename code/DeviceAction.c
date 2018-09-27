#include "DeviceAction.h"
#include "STC15W4K58S4.h"
#include "Delay.h"


sbit LED      =  P5 ^ 4;         		  // LED��
sbit Buzzer    =  P1 ^ 7;           	// ������

sbit MOTORIN1 =  P1 ^ 5;          		// ���Ƶ������
sbit MOTORIN2 =  P1 ^ 6;          	  // ���Ƶ������


///���Ƶ��ת�� ��������

void Motor_Actions_Status(unsigned char motor1, unsigned char motor2){
	
	MOTORIN1 = motor1;
	MOTORIN2 = motor2;
	
}

///LED  ���ƿ���

void Led_Actions_Status(unsigned char status){

	if(status){
		LED = 1;
	}else{
		LED = 0;
	}
	
}

///Buzzer   1��  0�� 

void Buzzer_Actions_Status(unsigned char status){

	if(status){
		Buzzer = 0;
	}else{
		Buzzer = 1;
	}

}

///LED��˸  ����  ʱ��

void Led_Actions_NumAndMS(unsigned char  number,unsigned char ms){
	
	if(number>0 && ms>0){
	
		while(number){
				number--;
				LED = 1;
				DELAY_MS(ms);
				LED = 0;
				if(number){
				   DELAY_MS(ms);
				}
		}
	}
}

///Buzzer����  ����  ʱ��

void Buzzer_Actions_NumAndMS(unsigned char  number,unsigned char ms){
	
	if(number>0 && ms>0){
	
		while(number){
				number--;
				Buzzer = 0;
				DELAY_MS(ms);
				Buzzer = 1;
				if(number){
				   DELAY_MS(ms);
				}
		}
	}
}

///LED��˸  Buzzer����  ����  ʱ��

void LedAndBuzzer_Actions_NumAndMS(unsigned char  number,unsigned char ms){
	
	if(number>0 && ms>0){
	
		while(number){
				number--;
				LED = 1;
				Buzzer = 0;
				DELAY_MS(ms);
				LED = 0;
				Buzzer = 1;
				if(number){
				   DELAY_MS(ms);
				}
		}
	}
}
