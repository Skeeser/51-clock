#include <REGX52.H>

sbit OneWire_DQ=P3^7;

 unsigned char OneWire_Init()
 {
	 unsigned char i;
	 unsigned char AckBit;
	 OneWire_DQ=1;
	 OneWire_DQ=0;
	
	 i = 247;while (--i);
	OneWire_DQ=1; 
	 i = 32;while (--i);
	 AckBit=OneWire_DQ;
	 i = 247;while (--i);
	 
	 return AckBit;
 }
 
 void OneWire_SendBit(unsigned char Bit)
 {unsigned char  i;
	 OneWire_DQ=0;
	 i = 4;while (--i);
	 
	 OneWire_DQ=Bit;
	 i = 24;
	while (--i);OneWire_DQ=1;
	 
 }
 
 unsigned char OneWire_ReceiveBit()
 {
	 unsigned char i;
	 unsigned char Bit;
	 
	 OneWire_DQ=0 ;
	  i = 2;while (--i);
	 OneWire_DQ=1 ;
	  i = 2;while (--i);
	 	Bit=OneWire_DQ;
	  i = 24;while (--i);
	 return Bit;
 }
 
 void OneWire_SendByte(unsigned char Byte)
 {
	 unsigned char i;
	 for(i = 0;i<8;i++)
	 {
		 OneWire_SendBit(Byte&(0x01<<i));		 
	 }
 }
 
 unsigned char OneWire_ReceiveByte(void)
 {
	 unsigned char i;
	 unsigned char Byte=0x00;
	 for(i = 0;i<8;i++)
	 {
		 if(OneWire_ReceiveBit()){ Byte|=(0x01<<i);} 
	 }
	 return Byte;
 }