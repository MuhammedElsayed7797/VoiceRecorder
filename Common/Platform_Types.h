/*
 * Platform_Types.h
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 */

#ifndef PLATFORM_TYPES_H_
#define PLATFORM_TYPES_H_

/***********************************************************************************/


/* 8.3 */
#ifndef TRUE
#define TRUE 	0x01u
#endif

#ifndef FALSE
#define FALSE 	0x0u
#endif

#define CPU_TYPE_8 		8
#define CPU_TYPE_16 	16
#define CPU_TYPE_32 	32
#define CPU_TYPE_64 	64

#define MSB_FIRST 		0
#define LSB_FIRST 		1

#define HIGH_BYTE_FIRST 0
#define LOW_BYTE_FIRST 	1


#define	NULL_PTR	((void*) 0x0)

/***********************************************************************************/


/* 8.2 */
typedef unsigned char 		boolean;

typedef signed char 		sint8;
typedef unsigned char 		uint8;

typedef signed short 		sint16;
typedef unsigned short 		uint16;

typedef signed long 		sint32;
typedef unsigned long 		uint32;

typedef signed long long 	sint64;
typedef unsigned long long 	uint64;

/***********************************************************************************/

/* its 32 bit due to AVR is faster with dealing with 8 bit
 * but least mean it won't overflow at uint8 range but it logically used for
 * range < 256 */
typedef unsigned char 		uint8_least;
typedef unsigned short 		uint16_least;
typedef unsigned long 		uint32_least;

typedef signed char 		sint8_least;
typedef signed short 		sint16_least;
typedef signed long 		sint32_least;

typedef float 				float32;
typedef double 				float64;

/***********************************************************************************/


/* 7.2 & 7.3*/
#define CPU_TYPE			CPU_TYPE_8
#define CPU_BIT_ORDER		LSB_FIRST
#define CPU_BYTE_ORDER 		LOW_BYTE_FIRST


/***********************************************************************************/

#endif /* PLATFORM_TYPES_H_ */
