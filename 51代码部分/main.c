#include <REGX52.H>
#include "LCD1602.h"
#include "DS1302.h"
#include "delay.h"
#include "key.h"
#include "Timer1.h"
#include "CTimer0.h"
#include "IR.h"
#include "Int0.h"
#include "DS18B20.h"
#include "OneWire.h"
#include "UART.h"

//自动对时
unsigned char serialtime[] = {0,0,0,0,0,0};
unsigned char ACTkey;
unsigned char ACKFlag;

//红外
idata unsigned int IR_Time;//下降沿到下一个下降沿的间隔时长
idata unsigned char IR_State;//状态位
idata unsigned char IR_Data[4];
idata unsigned char IR_pData;
idata unsigned char IR_DataFlag;//数据标志位 判断是否接收到数据
idata unsigned char IR_RepeatFlag;//重复标志位 判断是否为重复信号
idata unsigned char IR_Addr;
idata unsigned char IR_Command;

//闹钟
idata signed char AL_Time[]={0,0,0,0,0,0};
idata unsigned char ALSet;
idata unsigned char AWSet;
idata unsigned char ANum;

//音乐
#define SPEED 500
unsigned int code FreqTable[]={
63625,63732,63833,63929,64019,64104,64185,64260,64332,64400,64463,64524,
64580,64634,64685,64732,64778,64820,64860,64898,64934,64968,65000,65030,
65058,65085,65110,65134,65157,65178,65198,65217,65235,65252,65268,65283};

unsigned char code Music[]={
	7,2,
	7,2,
	9,4,
	7,4,
	12,4,
	11,8,
	7,2,
	7,2,
	9,4,
	7,4,
	14,4,
	12,8,
	7,4,
	7,4,
	19,4,
	16,4,
	12,4,
	11,4,
	9,4,
	17,6,
	17,4,
	16,4,
	12,4,
	14,4,
	12,8
};

unsigned char FreqSelect,MusicSelect;

//？？？
unsigned char Bsec=10;

//主时钟
unsigned char keynum,CLSet;//接收按键返回值，模式，时钟位选
unsigned char MOD=1;
signed char WSet;//星期位

//温度检测
float T;

//秒表
unsigned char WatchMode;
unsigned char micosec1=0,micosec2=0,sec=0,min=0,hour=0;
unsigned char Wcount;
unsigned char IRNum;

//倒计时
unsigned char TimeSelect,TimeSetFlag;
unsigned char CDcount,CDmode,CDMod;
char COUNTDOWN_Time[]={0,0,0,0};

//断电走时
unsigned char FLAG;

//闪烁工具数
unsigned int count,TSet;

//循环工具数
unsigned int ti;
sbit bee=P2^0;

//缓慢切换 试用功能
unsigned int Slow;

//红外灵敏度提高
unsigned char Clean;

//按键灵敏度提高
unsigned char keyflag=1;
idata unsigned char keylm;
idata unsigned char its,jts;
idata unsigned char ias,jas;
idata unsigned char ics,jcs;

//背光控制
unsigned char backlight;

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
void code TimerShow()
{		
		
		//清空调时位选
		DS_ReadTime();//每次进入走时模式，先读取一次时钟走时起点
		LCD_ShowString(1,13," ");
		LCD_ShowNum(1,1,DS_Time[0],2);
		LCD_ShowString(1,3,"-");
		LCD_ShowNum(1,4,DS_Time[1],2);
		LCD_ShowString(1,6,"-");
		LCD_ShowNum(1,7,DS_Time[2],2);
		LCD_ShowNum(2,1,DS_Time[3],2);
		LCD_ShowString(2,3,":");
		LCD_ShowNum(2,4,DS_Time[4],2);
		LCD_ShowString(2,6,":");
		LCD_ShowNum(2,7,DS_Time[5],2);
		LCD_ShowString(1,9," ");
		LCD_ShowString(2,9," ");
		LCD_ShowString(2,10,"   ");
		if(DS_Time[3]==0&&DS_Time[4]==0&&DS_Time[5]==0){delayms(600);WSet++;}//参数勿改
		if(WSet>6){WSet=0;}
		LCD_ShowString(1,10,DS_DAY[WSet]);
		
		if(keylm==2||keylm==3){keyflag=0;MOD=0;}
		if(ANum==0){LCD_ShowString(1,14,"OFF");}else{LCD_ShowString(1,14,"ON ");}
		delayms(5);
		if(DS_Time[4]==0&&DS_Time[5]==0)//整点报时
		{
			for(ti=0;ti<80;ti++)
			{
				bee=1;delayms(2);
				bee=0;delayms(2);
			}
		}
		
		//背光控制，每按一次按键四或红外，将backlight的值01不断转换
		if(keylm==4||IR_Command==0x45) 
		{ 
		keyflag=0;
		if(backlight==1) backlight=0;
		else backlight=1;
		IR_Command=0;		
		}
}



void code TimerSet()	
{	
	LCD_ShowString(1,9," ");//消影
	LCD_ShowString(1,13," ");
	LCD_ShowString(2,9,"   ");

	if(IR_Command==0x43||keylm==2)
	{
		keyflag=0;
		CLSet++;//调时位选
		CLSet%=7;
		IR_Command=0;
	}		
	else if(IR_Command==0x40)
	{
		CLSet--;//调时位选
		if(CLSet==-1){CLSet=6;}
		IR_Command=0;
	}		
	else if(IR_Command==0x09||keylm==3)//加
	{
		keyflag=0;
		if(CLSet==6){WSet++;}
		else{DS_Time[CLSet]++;if(CLSet==2)WSet++;}//操作时间数组
		if((DS_Time[1]==1||DS_Time[1]==3||DS_Time[1]==5||DS_Time[1]==7||DS_Time[1]==8||DS_Time[1]==10||DS_Time[1]==12)&&DS_Time[2]>31){DS_Time[2]=1;}
		if((DS_Time[1]==4||DS_Time[1]==6||DS_Time[1]==9||DS_Time[1]==11)&&DS_Time[2]>30){DS_Time[2]=1;}
		if(DS_Time[1]>12){DS_Time[1]=1;DS_Time[0]++;}
		if(DS_Time[2]>28&&DS_Time[0]%4!=0&&DS_Time[1]==2){DS_Time[2]=1;}
		if(DS_Time[2]>29&&DS_Time[0]%4==0&&DS_Time[1]==2){DS_Time[2]=1;}
		if(DS_Time[3]>23){DS_Time[3]=0;}
		if(DS_Time[4]==60){DS_Time[4]=0;}
		if(DS_Time[5]==60){DS_Time[5]=0;}
		if(WSet>6){WSet=0;}
		IR_Command=0;
	}
	else if(IR_Command==0x15||keylm==4)//减
	{
		keyflag=0;
		if(CLSet==6){WSet--;}
		else{DS_Time[CLSet]--;if(CLSet==2)WSet--;}//操作时间数组
		if((DS_Time[1]==1||DS_Time[1]==3||DS_Time[1]==5||DS_Time[1]==7||DS_Time[1]==8||DS_Time[1]==10||DS_Time[1]==12)&&DS_Time[2]<1){DS_Time[2]=31;}
		if((DS_Time[1]==4||DS_Time[1]==6||DS_Time[1]==9||DS_Time[1]==11)&&DS_Time[2]<1){DS_Time[2]=30;}
		if(DS_Time[1]<1){DS_Time[1]=12;DS_Time[0]--;}
		if(DS_Time[2]<1&&DS_Time[0]%4!=0&&DS_Time[1]==2){DS_Time[2]=28;}
		if(DS_Time[2]<1&&DS_Time[0]%4==0&&DS_Time[1]==2){DS_Time[2]=29;}
		if(DS_Time[3]<0){DS_Time[3]=23;}
		if(DS_Time[4]==-1){DS_Time[4]=59;}
		if(DS_Time[5]==-1){DS_Time[5]=59;}
		if(WSet==-1){WSet=6;}
		IR_Command=0;
	}
	else{Clean=1;}
		//闪烁功能
		if(CLSet==0&&TSet==0){LCD_ShowString(1,1,"  ");}else{LCD_ShowNum(1,1,DS_Time[0],2);}
		
		if(CLSet==1&&TSet==0){LCD_ShowString(1,4,"  ");}else{LCD_ShowNum(1,4,DS_Time[1],2);}
		
		if(CLSet==2&&TSet==0){LCD_ShowString(1,7,"  ");}else{LCD_ShowNum(1,7,DS_Time[2],2);}
		
		if(CLSet==3&&TSet==0){LCD_ShowString(2,1,"  ");}else{LCD_ShowNum(2,1,DS_Time[3],2);}
		
		if(CLSet==4&&TSet==0){LCD_ShowString(2,4,"  ");}else{LCD_ShowNum(2,4,DS_Time[4],2);}
		
		if(CLSet==5&&TSet==0){LCD_ShowString(2,7,"  ");}else{LCD_ShowNum(2,7,DS_Time[5],2);}
		
		if(CLSet==6&&TSet==0){LCD_ShowString(1,10,"   ");}else{LCD_ShowString(1,10,DS_DAY[WSet]);}
		
		//将延时总共分为25份，每份检测一次按键的值，从而提高按键的灵敏度
		if(TSet==1){for(its=1;its<=25;its++){delayms(10);keylm=keystay();}TSet=0;}
		else if(TSet==0){for(jts=1;jts<=25;jts++){delayms(20);keylm=keystay();}TSet=1;}
		LCD_ShowString(1,3,"-");LCD_ShowString(1,6,"-");LCD_ShowString(2,3,":");LCD_ShowString(2,6,":");
		DS_SetTime();//重新设置走时起点
}
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
void  AlarmRun()
{
	if(DS_Time[0]==AL_Time[0]&&DS_Time[1]==AL_Time[1]&&DS_Time[2]==AL_Time[2]&&DS_Time[3]==AL_Time[3]&&DS_Time[4]==AL_Time[4]&&DS_Time[5]==AL_Time[5]&&ANum==1)
	{
		while(1)
		{
			for(ti=0;ti<25;ti++){bee=1;delayms(10);bee=0;delayms(5);}
			LCD_ShowString(1,1,"TIME IS UP      ");
			if(TSet==0){LCD_ShowString(2,1,"                   ");}else{LCD_ShowString(2,1,"Press pause key");}
			if(TSet==1){delayms(450);TSet=0;}
			else if(TSet==0){delayms(450);TSet=1;}
			if(keylm==1||IR_Command==0x44){keyflag=0;ANum=0;break;}
		}
		IR_Command=0;
	}
	
}
	
void  AlarmGet()
{
	for(ti=0;ti<7;ti++)
	{
			AL_Time[ti]=DS_Time[ti];
	}
	AWSet=WSet;ALSet=4;
	ANum++;ANum%=2;
	delayms(750);
	MOD=1;
}

void  AlarmSet()//设置的函数后加一句 IR_RepeatFlag=0 
{
	LCD_ShowString(1,9," ");//消影
	LCD_ShowString(1,13," ");
	LCD_ShowString(2,9,"   ");
	if(IR_Command==0x43||keylm==2)
	{
		keyflag=0;
		ALSet++;//调时位选
		ALSet%=6;
		IR_Command=0;
	}		
	else if(IR_Command==0x40)
	{
		ALSet--;//调时位选
		if(ALSet==-1){ALSet=5;}
		IR_Command=0;
	}		
	else if(IR_Command==0x09||keylm==3)//加
	{
		keyflag=0;
		if(ALSet==6){AWSet++;}
		else{AL_Time[ALSet]++;if(ALSet==2)AWSet++;}//操作时间数组
		if((AL_Time[1]==1||AL_Time[1]==3||AL_Time[1]==5||AL_Time[1]==7||AL_Time[1]==8||AL_Time[1]==10||AL_Time[1]==12)&&AL_Time[2]>31){AL_Time[2]=1;}
		if((AL_Time[1]==4||AL_Time[1]==6||AL_Time[1]==9||AL_Time[1]==11)&&AL_Time[2]>30){AL_Time[2]=1;}
		if(AL_Time[1]>12){AL_Time[1]=1;AL_Time[0]++;}
		if(AL_Time[2]>28&&AL_Time[0]%4!=0&&AL_Time[1]==2){AL_Time[2]=1;}
		if(AL_Time[2]>29&&AL_Time[0]%4==0&&AL_Time[1]==2){AL_Time[2]=1;}
		if(AL_Time[3]>23){AL_Time[3]=0;}
		if(AL_Time[4]==60){AL_Time[4]=0;}
		if(AL_Time[5]==60){AL_Time[5]=0;}
		if(AWSet>6){AWSet=0;}
		IR_Command=0;
	}
	else if(IR_Command==0x15||keylm==4)//减
	{
		keyflag=0;
		if(ALSet==6){AWSet--;}
		else{AL_Time[ALSet]--;if(ALSet==2)AWSet--;}//操作时间数组
		if((AL_Time[1]==1||AL_Time[1]==3||AL_Time[1]==5||AL_Time[1]==7||AL_Time[1]==8||AL_Time[1]==10||AL_Time[1]==12)&&AL_Time[2]<1){AL_Time[2]=31;}
		if((AL_Time[1]==4||AL_Time[1]==6||AL_Time[1]==9||AL_Time[1]==11)&&AL_Time[2]<1){AL_Time[2]=30;}
		if(AL_Time[1]<1){AL_Time[1]=12;}
		if(AL_Time[2]<1&&AL_Time[0]%4!=0&&AL_Time[1]==2){AL_Time[2]=28;}
		if(AL_Time[2]<1&&AL_Time[0]%4==0&&AL_Time[1]==2){AL_Time[2]=29;}
		if(AL_Time[3]<0){AL_Time[3]=23;}
		if(AL_Time[4]==-1){AL_Time[4]=59;}
		if(AL_Time[5]==-1){AL_Time[5]=59;}
		if(AWSet==-1){AWSet=6;}
		IR_Command=0;
	}
	else{Clean=1;}
	
		if(ALSet==0&&TSet==0){LCD_ShowString(1,1,"  ");}else{LCD_ShowNum(1,1,AL_Time[0],2);}//配合定时器实现闪烁（闪烁后于调时，显示当前设置时间） 
		
		if(ALSet==1&&TSet==0){LCD_ShowString(1,4,"  ");}else{LCD_ShowNum(1,4,AL_Time[1],2);}
		
		if(ALSet==2&&TSet==0){LCD_ShowString(1,7,"  ");}else{LCD_ShowNum(1,7,AL_Time[2],2);}
		
		if(ALSet==3&&TSet==0){LCD_ShowString(2,1,"  ");}else{LCD_ShowNum(2,1,AL_Time[3],2);}
		
		if(ALSet==4&&TSet==0){LCD_ShowString(2,4,"  ");}else{LCD_ShowNum(2,4,AL_Time[4],2);}
		
		if(ALSet==5&&TSet==0){LCD_ShowString(2,7,"  ");}else{LCD_ShowNum(2,7,AL_Time[5],2);}
		
		if(ALSet==6&&TSet==0){LCD_ShowString(1,10,"   ");}else{LCD_ShowString(1,10,DS_DAY[AWSet]);}
		
		LCD_ShowString(1,3,"-");LCD_ShowString(1,6,"-");LCD_ShowString(2,3,":");LCD_ShowString(2,6,":");
		
		if(TSet==1){for(ias=1;ias<=25;ias++){delayms(10);keylm=keystay();}TSet=0;}
		else if(TSet==0){for(jas=1;jas<=25;jas++) {delayms(20);keylm=keystay();}TSet=1;}
		
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------


void code TEMShow()
{
		//显示温度字符并消影
		LCD_ShowString(1,1,"Temperature:     ");
		LCD_ShowString(2,9,"     ");
		DS18B20_ConvertT();
		T = DS18B20_ReadT();
		
		//判断温度是否为负数，若为负数，将温度变正数并加上负的字符
		if(T<0)
		{
			LCD_ShowChar(2,1,'-');
			T=-T;
		}
		else 
		{
			LCD_ShowChar(2,1,'+');
		}
		//显示温度的整数部分
		LCD_ShowNum(2,2,T,3);
		LCD_ShowChar(2,5,'.');
		//显示温度的小数部分，并控制精度
		LCD_ShowNum(2,6,(unsigned long)(T*100)%100,2);
		LCD_ShowChar(2,8,'C');
			
}


//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

void code StopWatch()
	{		
		//秒表显示和消影
		delayms(1);
		keylm=keystay();
		LCD_ShowString(1,1,"StopWatch:            ");
		LCD_ShowString(2,3,":");
		LCD_ShowString(2,6,":");
		LCD_ShowString(2,9,":");
		//秒表模式一是开始计时，模式二是暂停，模式三是清除
		if(IR_Command==0x45||keylm==2){keyflag=0;WatchMode=1;}
		else if(IR_Command==0x44||keylm==3){keyflag=0;WatchMode=2;}
		else if(IR_Command==0x07||keylm==4){keyflag=0;WatchMode=3;}
		else{Clean=1;}
		if(WatchMode==1)
		{	
			micosec1++;
			if(micosec1%2){micosec2++;micosec2++;}
			if(micosec1>=10){micosec1=0;sec++;}
			if(sec>=60){sec=0;min++;}
			if(min>=60){min=0;hour++;}	
			if(hour>=24){hour=0;}
		}						
		else if(WatchMode==2){WatchMode=0;}
		else if(WatchMode==3){micosec2=0;micosec1=0;sec=0;min=0;hour=0;TL1=0x00;TH1=0x00;}
		LCD_ShowNum(2,1,hour,2);
		LCD_ShowNum(2,4,min,2);
		LCD_ShowNum(2,7,sec,2);
		LCD_ShowNum(2,10,micosec1,1);
		LCD_ShowNum(2,11,micosec2,1);
		
		LCD_ShowNum(2,14,MOD,2);
		
		
	}
	
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

void CDShow()//倒计时的显示
{
		//倒计时显示和消影
		LCD_ShowString(1,1,"count down:      ");
		LCD_ShowString(2,3,":");
		LCD_ShowString(2,6,":");
		LCD_ShowString(2,9,"   ");
		delayms(900);
		CDcount=1;
		//倒计时的走时
		if(CDcount)
			{
				if(COUNTDOWN_Time[0]!=0||COUNTDOWN_Time[1]!=0||COUNTDOWN_Time[2]!=0)
				{	COUNTDOWN_Time[2]--	;
					if(COUNTDOWN_Time[2]<0) {COUNTDOWN_Time[2]=59;COUNTDOWN_Time[1]--;}
					if(COUNTDOWN_Time[1]<0) 
					{
						if(COUNTDOWN_Time[0]!=0){COUNTDOWN_Time[1]=59;COUNTDOWN_Time[0]--;}
						else if(COUNTDOWN_Time[0]==0) COUNTDOWN_Time[1]=0;
					}
					if(COUNTDOWN_Time[0]<0) COUNTDOWN_Time[0]=0;
				}
			}	
		CDcount=0;
		LCD_ShowNum(2,1,COUNTDOWN_Time[0],2);
	  LCD_ShowNum(2,4,COUNTDOWN_Time[1],2);
	  LCD_ShowNum(2,7,COUNTDOWN_Time[2],2);
		if(COUNTDOWN_Time[0]==0&&COUNTDOWN_Time[1]==0&&COUNTDOWN_Time[2]==0)
		{
			for(ti=0;ti<30;ti++)
			{
				bee=1;delayms(2);//倒计时时间到了，蜂鸣器发出响声
				bee=0;delayms(2);
			}
			TimeSelect=0;
		}
		LCD_ShowString(2,9,"   ");
}

void CDSet()//倒计时的设置
{
	//显示与消影
	LCD_ShowString(1,1,"count down:      ");
	LCD_ShowString(2,3,":");
	LCD_ShowString(2,6,":");
	LCD_ShowString(2,9,"   ");
	if(TimeSelect==0&&TimeSetFlag==1){LCD_ShowString(2,1,"  ");}
	else{LCD_ShowNum(2,1,COUNTDOWN_Time[0],2);}
	if(TimeSelect==1&&TimeSetFlag==1){LCD_ShowString(2,4,"  ");}
	else{LCD_ShowNum(2,4,COUNTDOWN_Time[1],2);}
	if(TimeSelect==2&&TimeSetFlag==1){LCD_ShowString(2,7,"  ");}
	else{LCD_ShowNum(2,7,COUNTDOWN_Time[2],2);}
	
	//按键2是位选，选择设置倒计时的位
	//COUNTDOWN_Times数组有四个元素，当按2进入COUNTDOWN_Times[3]时，自动进行倒计时
	if(keylm==2||IR_Command==0x43)
	{
		keyflag=0;
		TimeSelect++;
		TimeSelect%=4;
		IR_Command=0;
	}
	//按键3和4进行加减设置
	else if(keylm==3||IR_Command==0x09)
	{
		keyflag=0;
		COUNTDOWN_Time[TimeSelect]++;
		if(COUNTDOWN_Time[0]>23){COUNTDOWN_Time[0]=0;}
		if(COUNTDOWN_Time[1]>59){COUNTDOWN_Time[1]=0;}
		if(COUNTDOWN_Time[2]>59){COUNTDOWN_Time[2]=0;}
		IR_Command=0;
	}	
	
	else if(keylm==4||IR_Command==0x15)
	{
		keyflag=0;
		COUNTDOWN_Time[TimeSelect]--; 
		if(COUNTDOWN_Time[0]<0){COUNTDOWN_Time[0]=23;}
		if(COUNTDOWN_Time[1]<0){COUNTDOWN_Time[1]=59;}
		if(COUNTDOWN_Time[2]<0){COUNTDOWN_Time[2]=59;}
		IR_Command=0;
	}	
	else{Clean=1;}
		//闪烁，同时进行了按键灵敏度提高
		if(TimeSetFlag==1){for(ics=1;ics<=25;ics++){delayms(10);keylm=keystay();}TimeSetFlag=0;}
		else if(TimeSetFlag==0){for(jcs=1;jcs<=25;jcs++){delayms(15);keylm=keystay();}TimeSetFlag=1;}
		
	
	
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
void code ACT()
{
		
		LCD_ShowString(2,3,":");
		LCD_ShowString(2,6,":");
		//分别显示串口时间数组的值
		LCD_ShowNum(2,1,serialtime[0],1);
		LCD_ShowNum(2,2,serialtime[1],1);
		LCD_ShowNum(2,4,serialtime[2],1);
		LCD_ShowNum(2,5,serialtime[3],1);
		LCD_ShowNum(2,7,serialtime[4],1);
		LCD_ShowNum(2,8,serialtime[5],1);
		
		ACTkey = keystay();//获取键码
		LCD_ShowString(1,1,"AutoCorrect YorN");
		//按键2的作用是校正串口传来时间数据的位置
	  if(ACTkey==2||IR_Command==0x19) {keyflag=0;ACKFlag = 1; IR_Command=0;}
		//按键3的作用是将单片机的ds1302的时间进行修改
		if(ACTkey==3||IR_Command==0x0D)
		{
			keyflag=0;
			DS_Time[3] = serialtime[0] * 10 + serialtime[1];
			DS_Time[4] = serialtime[2] * 10 + serialtime[3];
			DS_Time[5] = serialtime[4] * 10 + serialtime[5];
			LCD_ShowString(2,10,"ok!");//修改成功显示ok
			DS_SetTime();
		}
		

}
	

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

void Bomb()
{
		LCD_ShowString(1,1,"There is a bomb:      ");
		LCD_ShowString(2,1,"00:");
		LCD_ShowString(2,4,"00:");
		LCD_ShowString(2,9,"   ");
		while(1)
		{
			delayms(900);
			Bsec--;
			LCD_ShowNum(2,7,Bsec,2);
			if(Bsec==0){Bsec=10;MOD=9;break;}
		}
}

void code MusicPlay()
{
			if(Music[MusicSelect]!=0xFF)	//如果不是停止标志位
		{
			FreqSelect=Music[MusicSelect];	//选择音符对应的频率
			MusicSelect++;
			delayms(SPEED/4*Music[MusicSelect]);	//选择音符对应的时值
			MusicSelect++;
			TR1=0;
			delayms(5);
			TR1=1;
		}
		else	//如果是停止标志位
		{
			while(1);
		}

}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

void main()
{
	LCD_Init();DS_Init();CTimer0_Init();Timer1_Init();IR_Init();Int0_Init();UART_Init();	
	FLAG=DS_ReadByte(0XC0);
	//断电走时功能，防止再次通电时ds1302被初始化
	if(FLAG==0)
	{
	DS_SetTime();
	DS_WriteByte(0xc0,1);
	} 
	while(1)
	{
			keylm=keystay();
			//在主函数中按键一进行模式切换
			if(IR_Command==0x46||keylm==1)
			{
				keyflag=0;//每次有按键按下要把keyflag清零
				MOD++;
				if(MOD>7){MOD=1;}
				IR_Command=0;
			}
			
		  if(IR_Command==0x16){MOD=0;IR_Command=0;}//可以封装成函数
			else if(IR_Command==0x0C){MOD=1;IR_Command=0;}//但是比较懒不想改了
			else if(IR_Command==0x18){MOD=2;IR_Command=0;}
			else if(IR_Command==0x5E){MOD=3;IR_Command=0;}
			else if(IR_Command==0x08){MOD=4;IR_Command=0;}
			else if(IR_Command==0x1C){MOD=5;IR_Command=0;}
			else if(IR_Command==0x5A){MOD=6;IR_Command=0;}
			else if(IR_Command==0x42){MOD=7;IR_Command=0;}
			else if(IR_Command==0x52){MOD=8;IR_Command=0;}
			else{Clean=1;}
			LCD_ShowNum(2,14,MOD,2);
			if(MOD==9){ET1=1;}else{ET1=0;}
			switch(MOD)
			{	
				case 0:delayms(250);AlarmGet();break;
				case 1:TimerShow();AlarmRun();break;
				case 2:TimerSet();break;
				case 3:AlarmSet();break;
				case 4:TEMShow();break;
				case 5:StopWatch();break;
				case 6:if(TimeSelect==3){CDShow();}else{CDSet();}break;	
				case 7:ET1=0;ACT();break;
				case 8:Bomb();break;
				case 9:ET1=1;MusicPlay();break;	
			}
			if(Clean==0){IR_Command=0;}else{Clean=0;}//红外灵敏度提高
	}
	

}


//-------------------------------------------------------------------------------------------------
void Timer1() interrupt 3
{
	if(MOD==9)
	{
		TL1 = FreqTable[FreqSelect]%256;	
		TH1 = FreqTable[FreqSelect]/256;
		count++;
		if(count%2)bee=~bee;
	}
	
}
//--------------------------------------------------------------------------------------------------
void IR_Int0() interrupt 0
{
	if(IR_State==0)//第一次下降沿设置定时器初值 启动定时器 并将状态置为1
	{
		CTimer0_SCounter(0);
		CTimer0_R(1);
		IR_State=1;
	}
	else if(IR_State==1)//状态为1时 判断信号的类型 
	{
		IR_Time=CTimer0_GCounter();
		CTimer0_SCounter(0);
		if(IR_Time>13500-500&&IR_Time<13500+500)//若是start信号 进入状态2
		{
			
			IR_State=2;
		}
		else if(IR_Time>11250-500&&IR_Time<11250+500)//若是重复信号 回到状态0
		{
			IR_RepeatFlag=1;
			IR_State=0;
		}
	}
	else if(IR_State==2)//switch语句更直观
	{
		IR_Time=CTimer0_GCounter();
		CTimer0_SCounter(0);
		if(IR_Time>1120-500&&IR_Time<1120+500)//每次进入中断状态2 判断为0或为1
		{
			IR_Data[IR_pData/8]&=~(0x01<<(IR_pData%8));//数组的顺序符合信号的顺序 即地址 地址反码 指令 指令反码											
			IR_pData++;											       
		}
		else if(IR_Time>2250-500&&IR_Time<2250+500)
		{
			IR_Data[IR_pData/8]|=(0x01<<(IR_pData%8));
			IR_pData++;   
		}
		else
		{
				IR_pData=0;
				IR_State=1;
		}
		if(IR_pData>=32)//说明已经完成三十二位解码 清空标志位 并检验
		{
				IR_pData=0;
				if(IR_Data[0]==~IR_Data[1]&&IR_Data[2]==~IR_Data[3])//通过原码和反码判断数据是否有误
				{
					IR_Addr=IR_Data[0];//获取地址
					IR_Command=IR_Data[2];//获取指令
					IR_DataFlag=1;
				}
				CTimer0_R(0);
				IR_State=0;
		}
	}
	if(MOD==9)
	{
		if(FreqTable[FreqSelect])	//如果不是休止符
		{
		TL1 = FreqTable[FreqSelect]%256;		//设置定时初值
		TH1 = FreqTable[FreqSelect]/256;		//设置定时初值
		bee=~bee;//翻转蜂鸣器IO口
		}
	}
}

void UART_Routine() interrupt 4
{
static unsigned int i = 0;
	if(RI==1)			
	{
	serialtime[i] = SBUF;//将串口的SBUF存入串口时间数组中
	i++;
	if(i==6) i=0;
	ACTkey=keystay();
	//校正时间的位置
	if(ACTkey==2||IR_Command==0x19||ACKFlag == 1) {i=0;keyflag=0; ACKFlag = 0;IR_Command=0;}
		RI=0;	
	}
}


