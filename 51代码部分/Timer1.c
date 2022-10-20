#include <REGX52.H>


void Timer1_Init()
{
	
	TMOD&=0x0F;
	TMOD|=0x10;
	TH1=0xFF;
	TL1=0x9C;
	TF1=0;
	TR1=1;
	EA=1;
	ET1=1;
	PT1=1;

}