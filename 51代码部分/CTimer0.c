#include <REGX52.H>
#include "CTimer0.h"

void CTimer0_Init()
{
	TMOD&=0xF0;
	TMOD|=0x01;
	TH0=0;
	TL0=0;
	TF0=0;
	TR0=0;//定时器不计时
}

void CTimer0_SCounter(unsigned int val)
{
	TH0=val/256;
	TL0=val%256;
}

unsigned int CTimer0_GCounter()
{
	return (TH0<<8)|TL0;
}

void CTimer0_R(unsigned char flag)
{
	TR0=flag;
}
