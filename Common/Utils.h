/*
 * 	Utils.h
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 *
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "Platform_Types.h"


#define SET_BIT(reg, n) 	(reg |= (1<<n))
#define CLR_BIT(reg, n) 	(reg &=  ~(1<<n))
#define TOG_BIT(reg, n) 	(reg ^=(1<<n))
#define GET_BIT(reg, n) 	((reg>>n)&1)


#define SET_NBL_LOW(reg) 	(reg |= (0x0F))
#define SET_NBL_HIGH(reg) 	(reg |= (0xF0))

#define RST_NBL_LOW(reg) 	(reg &= (0xF0))
#define RST_NBL_HIGH(reg) 	(reg &= (0x0F))

#define TOG_NBL_LOW(reg) 	(reg ^=(0x0F))
#define TOG_NBL_HIGH(reg) 	(reg ^=(0xF0))

#define GET_NBL_LOW(reg) 	((reg)&0x0F)
#define GET_NBL_HIGH(reg) 	((reg>>4)&0x0F)



#define SET_NBL_VAL_LOW(reg, val) 		(reg|=((reg &= (0xF0))|(val & 0x0F)))
#define SET_NBL_VAL_HIGH(reg, val) 		(reg|=((reg &= (0x0F))|(val & 0xF0)))


#define ASSIGN_BIT(reg,num,state)	reg =  (reg & ~(1 <<(num))) | (state<<(num))	/* assign 'state' to pin with 'num' inside 'reg' */


/***********************************************************************************/


/* used to ease access to registers of AVR */
typedef union
{
	struct
	{
		uint8 Bit_0 	: 	1 ;
		uint8 Bit_1 	: 	1 ;
		uint8 Bit_2 	: 	1 ;
		uint8 Bit_3 	: 	1 ;
		uint8 Bit_4 	: 	1 ;
		uint8 Bit_5 	: 	1 ;
		uint8 Bit_6 	: 	1 ;
		uint8 Bit_7 	: 	1 ;

	}RegisterBits;

	uint8 RegisterByte ;

} AVR_Register ;


/***********************************************************************************/


/* used for initialize all elements of array with value x */
#define VAL_1(x)        x
#define VAL_2(x)        VAL_1(x), VAL_1(x)
#define VAL_4(x)        VAL_2(x), VAL_2(x)
#define VAL_8(x)        VAL_4(x), VAL_4(x)
#define VAL_16(x)       VAL_8(x), VAL_8(x)
#define VAL_32(x)       VAL_16(x), VAL_16(x)
#define VAL_64(x)       VAL_32(x), VAL_32(x)
#define VAL_128(x)      VAL_64(x), VAL_64(x)
#define VAL_256(x)      VAL_128(x), VAL_128(x)

/***********************************************************************************/

#define E_OK 0x00u
#define E_NOT_OK 0x01u
typedef uint8 StatusType ; 	/* OSEK compliance */

/***********************************************************************************/


#endif /* UTILS_H_ */
