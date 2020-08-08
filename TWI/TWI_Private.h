/*
 * 	TWI_Private.h
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 *
 */

#ifndef TWI_TWI_PRIVATE_H_
#define TWI_TWI_PRIVATE_H_

/*****************************************************************************/


#define TWI_ISR			__vector_17

/* the interrupt function depend on vector sent to it go back to vector table   */
#define	ISR( vector )			void vector (void) __attribute__ ((signal,used, externally_visible)) ; \
								void vector (void)

/*****************************************************************************/

/* addresses of TWI registers */

#define TWBR_REG	( ( volatile AVR_Register * ) 0x20 )
#define TWSR_REG	( ( volatile AVR_Register * ) 0x21 )
#define TWAR_REG	( ( volatile AVR_Register * ) 0x22 )
#define TWDR_REG	( ( volatile AVR_Register * ) 0x23 )
#define TWCR_REG	( ( volatile AVR_Register * ) 0x56 )

/* port c register to enable internal pull up resistors */
#define DDRC_REG	( ( volatile AVR_Register * ) 0x34 )
#define PORTC_REG	( ( volatile AVR_Register * ) 0x35 )



/*****************************************************************************/




#endif /* TWI_TWI_PRIVATE_H_ */
