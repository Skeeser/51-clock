#include <REGX52.H>
#include "delay.h"

extern unsigned char keyflag;

unsigned char keystay()
{
	static unsigned char keynumber;
	
	if(P3_3==0){delayms(20);while(P3_3==0);delayms(20);keynumber=1;}
	if(P3_4==0){delayms(20);while(P3_4==0);delayms(20);keynumber=2;}
	if(P3_5==0){delayms(20);while(P3_5==0);delayms(20);keynumber=3;}
	if(P3_6==0){delayms(20);while(P3_6==0);delayms(20);keynumber=4;}

	if(keyflag==0) {keynumber=0;keyflag=1;}
	
	return keynumber;
}