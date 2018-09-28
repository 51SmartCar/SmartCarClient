#include "Delay.h"
#include <INTRINS.H>

// 参数: ms,要延时的ms数, 这里只支持1~255ms.  11059200UL自动适应主时钟.

void DELAY_MS(unsigned char ms){
    unsigned int i;
		do{
	      i = 11059200UL / 13000;
		  while(--i)	;
     }while(--ms);
}

void Delay10us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	i = 25;
	while (--i);
}
