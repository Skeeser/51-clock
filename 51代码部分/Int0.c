#include <REGX52.H>

void Int0_Init()
{
	IT0=1;//IT0=1时为下降沿中断  IT0=0时为低电平中断（可持续进入中断系统）
	IE0=0;
	EX0=1;
	EA=1;
	PX0=1;
}