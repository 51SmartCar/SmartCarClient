#ifndef __PUBLIC_H_
#define __PUBLIC_H_

#include "STC15W4K58S4.h"


#define uint8     unsigned char  
#define uint16    unsigned short int
#define uint32    unsigned int   

extern unsigned int PWMHEIGHT;
extern unsigned char Motor_Level;
extern unsigned int Num_Distance;

extern unsigned char CheckData(unsigned char *CHECK_DATA);
extern unsigned char uint_to_hex(unsigned int num);
extern unsigned char hex_to_dec(unsigned char);
extern unsigned char dec_to_hex(unsigned char);


#endif