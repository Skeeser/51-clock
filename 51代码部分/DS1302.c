#include <REGX52.H>
#include "delay.h"
#include "intrins.h"

#define SECOND  0x80 //写入的地址
#define MINUTE  0x82
#define HOUR    0x84
#define DATE    0x86
#define MONTH   0x88
#define DAY     0x8A
#define YEAR    0x8C
#define WP      0x8E


sbit DS_SCLK=P1^0;
sbit DS_IO=P1^1;
sbit DS_CE=P1^2;

signed char  DS_Time[]={21,11,9,6,59,55,0};//可优化的代码

char* DS_DAY[]={"SUN","MON","TUE","WED","THU","FRI","SAT"};

unsigned char DS_Mod[]={YEAR,MONTH,DATE,HOUR,MINUTE,SECOND,DAY};

unsigned char i;
void DS_Init()
{
	DS_CE=0;
	DS_SCLK=0;
	
}

void code DS_WriteByte(unsigned char command,Data)
{
	unsigned char i;
	DS_CE=1;
	for(i=0;i<8;i++)
	{	
		DS_IO=command&(0x01<<i);
		DS_SCLK=1;
		DS_SCLK=0;
	}
	for(i=0;i<8;i++)
	{	
		DS_IO=Data&(0x01<<i);
		DS_SCLK=1;
		DS_SCLK=0;
	}
	DS_CE=0;
}

unsigned char code DS_ReadByte(unsigned char command)
{
	unsigned char i,Data=0x00;
	command|=0x01;//最低位赋1 使读写指令共用宏定义
	DS_CE=1;
	for(i=0;i<8;i++)
	{	
		DS_IO=command&(0x01<<i);
		DS_SCLK=0;
		DS_SCLK=1;
	}
	DS_IO=0;
	for(i=0;i<8;i++)
	{
		DS_SCLK=1;
		DS_SCLK=0;
		if(DS_IO)
		{
			Data=Data|(0x01<<i);
		}
	}
	DS_CE=0;
	return Data;
}


unsigned int j;
void DS_SetTime()
{
	DS_WriteByte(WP,0x00);//解除写保护
	for(j=0;j<7;j++)
	{
		unsigned char temp;
		temp=DS_Time[j]/10*16+DS_Time[j]%10;//写入时转为BCD码
		DS_WriteByte(DS_Mod[j],temp);
	}
	DS_WriteByte(WP,0x80);//恢复写保护
}


void DS_ReadTime()
{
	for(i=0;i<7;i++)
	{
		unsigned char temp;
		temp=DS_ReadByte(DS_Mod[i]);
		DS_Time[i]=temp/16*10+temp%16;//读取时转为十进制
	}
	
	
	
}
