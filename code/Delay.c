#include "Delay.h"

// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms.  11059200UL�Զ���Ӧ��ʱ��.

void DELAY_MS(unsigned char ms){
    unsigned int i;
		do{
	      i = 11059200UL / 13000;
		  while(--i)	;
     }while(--ms);
}
