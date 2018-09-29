#include "Public.h"


/*

//1843

//1.0ms 0X2B32 ����45�Ƚ� 3��
//1. ms 0X3265 ����30�Ƚ� 2�� 
//1. ms 0X3998 ����15�Ƚ� 1��
//1.5ms 0X40CC ����
//1. ms 0X47FE ����15�Ƚ� 1��
//1. ms 0X4F31 ����30�Ƚ� 2�� 
//2.0ms 0X5665 ����45�Ƚ� 3��

*/

unsigned int PWMHEIGHT = 0X40CC;
unsigned char Motor_Level = 1;
unsigned int Num_Distance = 0;

///У������׼ȷ�� ��CRCУ��

unsigned char CheckData(unsigned char *CHECK_DATA) {

    unsigned char  CHECKSUM = CHECK_DATA[1]+CHECK_DATA[2]+CHECK_DATA[3]+CHECK_DATA[4]-0x01;

    return CHECKSUM;

}

//���ֵ65535
unsigned char uint_to_hex(unsigned int num){
	unsigned char a = num >>8;
	unsigned char b = num & 0xFF;
	unsigned char listArr[] = {0x00,0x00};
	listArr[1] = dec_to_hex(a);
	listArr[0] = dec_to_hex(b);
	return listArr;
}


/******************************************
*     ʮ����ת��Ϊʮ������  255                
*******************************************/

unsigned char dec_to_hex(unsigned char b)
{
	unsigned char hex_buf=0;
	unsigned char i=0; 
	unsigned char arr[2]={0,0};
 if(b>15)
	{   
		  i=1;//��־�Ŵ���15����λ��
			arr[0]=b%16;
			b=b/16;
			arr[1]=b%16;
	}
 else
 {
	 i=0;
	 arr[0]=b%16;
 }
 
	if(i==1)       
	{
		switch(arr[1])   
		{   
				 case 10: arr[i]=0X0A;	break;   
				 case 11: arr[i]=0X0B;	break;   
				 case 12: arr[i]=0X0C;  break;  
				 case 13: arr[i]=0X0D;	break; 
				 case 14: arr[i]=0X0E;	break;  
				 case 15: arr[i]=0X0F;	break;  
				 default: 	break; 
  	}  
	}
	if(i==0)
	{
		switch(arr[0])   
		{   
				 case 10: arr[i]=0X0A;	break;   
				 case 11: arr[i]=0X0B;	break;   
				 case 12: arr[i]=0X0C;  break;  
				 case 13: arr[i]=0X0D;	break; 
				 case 14: arr[i]=0X0E;	break;  
				 case 15: arr[i]=0X0F;	break;  
				 default: 	break; 
  	}  
	}
	
	hex_buf=((arr[1] << 4) | arr[0]);
	return hex_buf;
}


/******************************************
*      ʮ������ת��Ϊʮ����        255         
*******************************************/


unsigned char hex_to_dec(unsigned char a)
{
	  unsigned char dec_buf=0;
 	  unsigned char str[3]={0,0,0};
		unsigned char j=0;
		while(a)
		{
				str[j]=a%10;                                        
				a/=10;    
				j++;
		}
		dec_buf=str[2]*100+str[1]*10+str[0];
		return dec_buf;
}