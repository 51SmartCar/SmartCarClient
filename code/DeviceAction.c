#include "DeviceAction.h"
#include "STC15W4K58S4.h"
#include "Delay.h"


sbit LED      =  P5 ^ 4;         		  // LED灯
sbit Buzzer    =  P1 ^ 7;           	// 蜂鸣器

sbit MOTORIN1 =  P1 ^ 5;          		// 控制电机方向
sbit MOTORIN2 =  P1 ^ 6;          	  // 控制电机方向


///电机当前运行状态  前进1 后退2  刹车0
unsigned char Motor_CurrentStatus(void){
	
	if(MOTORIN1==1 && MOTORIN2==0){//前进
		return 1;
	}else if(MOTORIN1==0 && MOTORIN2==1){//后退
		return 2;
	}else if(MOTORIN1==0 && MOTORIN2==0){//刹车
		return 0;
	}
	
}


///控制电机转动 两个引脚

void Motor_Actions_Status(unsigned char motor1, unsigned char motor2){
	
	MOTORIN1 = motor1;
	MOTORIN2 = motor2;
	
}

///LED  控制开关

void Led_Actions_Status(unsigned char status){

	if(status){
		LED = 1;
	}else{
		LED = 0;
	}
	
}

///Buzzer   1关  0开 

void Buzzer_Actions_Status(unsigned char status){

	if(status){
		Buzzer = 0;
	}else{
		Buzzer = 1;
	}

}

///LED闪烁  次数  时间

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

///Buzzer声音  次数  时间

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

///LED闪烁  Buzzer声音  次数  时间

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
