#ifndef DEVICEACTION_H
#define DEVICEACTION_H

void Motor_Actions_Status(unsigned char motor1, unsigned char motor2);

void Led_Actions_Status(unsigned char status);

void Buzzer_Actions_Status(unsigned char status);

void Led_Actions_NumAndMS(unsigned char  number,unsigned char ms);

void Buzzer_Actions_NumAndMS(unsigned char  number,unsigned char ms);

void LedAndBuzzer_Actions_NumAndMS(unsigned char  number,unsigned char ms);

unsigned char Motor_CurrentStatus(void);

#endif
