// this file contains the implementation of the DIO functions 

#include "DIO_int.h"

/*This function initialize the DIO as user configration*/
void DIO_Init()
{
	assignport(DDRA,conc(PINDA0,PINDA1,PINDA2,PINDA3,PINDA4,PINDA5,PINDA6,PINDA7));
	assignport(DDRB,conc(PINDB0,PINDB1,PINDB2,PINDB3,PINDB4,PINDB5,PINDB6,PINDB7));
	assignport(DDRC,conc(PINDC0,PINDC1,PINDC2,PINDC3,PINDC4,PINDC5,PINDC6,PINDC7));
	assignport(DDRD,conc(PINDD0,PINDD1,PINDD2,PINDD3,PINDD4,PINDD5,PINDD6,PINDD7));
	assignport(PORTA,conc(PINA0,PINA1,PINA2,PINA3,PINA4,PINA5,PINA6,PINA7));
	assignport(PORTB,conc(PINB0,PINB1,PINB2,PINB3,PINB4,PINB5,PINB6,PINB7));
	assignport(PORTC,conc(PINC0,PINC1,PINC2,PINC3,PINC4,PINC5,PINC6,PINC7));
	assignport(PORTD,conc(PIND0,PIND1,PIND2,PIND3,PIND4,PIND5,PIND6,PIND7));
}

void DIO_SetPinDir(U8 LOC_U8_PinNum,U8 LOC_U8_PinDir)
{
	U8 port = LOC_U8_PinNum / 8;
	U8 LOC_U8_PinNumPortPer = LOC_U8_PinNum % 8;
	switch(port)
	{
		case 0 :
			assignbit(DDRA,LOC_U8_PinNumPortPer,LOC_U8_PinDir);
			break;
		case 1 :
			assignbit(DDRB,LOC_U8_PinNumPortPer,LOC_U8_PinDir);
			break;
		case 2 :
			assignbit(DDRC,LOC_U8_PinNumPortPer,LOC_U8_PinDir);
			break;
		case 3 :
			assignbit(DDRD,LOC_U8_PinNumPortPer,LOC_U8_PinDir);
			break;

	}	
}
void DIO_SetPinValue(U8 LOC_U8_PinNum,U8 LOC_U8_Pinvalue)
{
	U8 port = LOC_U8_PinNum / 8;
	U8 LOC_U8_PinNumPortPer = LOC_U8_PinNum % 8;
	switch(port)
	{
		case 0 :
			assignbit(PORTA,LOC_U8_PinNumPortPer,LOC_U8_Pinvalue);
			break;
		case 1 :
			assignbit(PORTB,LOC_U8_PinNumPortPer,LOC_U8_Pinvalue);
			break;
		case 2 :
			assignbit(PORTC,LOC_U8_PinNumPortPer,LOC_U8_Pinvalue);
			break;
		case 3 :
			assignbit(PORTD,LOC_U8_PinNumPortPer,LOC_U8_Pinvalue);
			break;
	}	
}
U8 DIO_GetPinValue(U8 LOC_U8_PinNum)
{
	U8 port = LOC_U8_PinNum / 8;
	U8 LOC_U8_PinNumPortPer = LOC_U8_PinNum % 8;
	U8 LOC_U8_RetPin = 0;
	switch(port)
	{
		case 0 :
			LOC_U8_RetPin =  getbit(PINA,LOC_U8_PinNumPortPer);
			break;
		case 1 :
			LOC_U8_RetPin =  getbit(PINB,LOC_U8_PinNumPortPer);
			break;
		case 2 :
			LOC_U8_RetPin =  getbit(PINC,LOC_U8_PinNumPortPer);
			break;
		case 3 :
			LOC_U8_RetPin =  getbit(PIND,LOC_U8_PinNumPortPer);
			break;
	}	
	return LOC_U8_RetPin;
}



