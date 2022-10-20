#ifndef __DS1302_H__
#define	__DS1302_H__
				
				extern signed char DS_Time[];
				extern unsigned char DS_Mod[];
				extern char* DS_DAY[];
				void DS_Init();
				void DS_WriteByte(unsigned char command,Data);
				unsigned char DS_ReadByte(unsigned char command);
				void DS_SetTime();
				void DS_ReadTime();
#endif