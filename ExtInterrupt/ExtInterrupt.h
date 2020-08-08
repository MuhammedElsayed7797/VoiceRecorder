/*
 * 	ExtInterrupt.h
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 *
 */




#ifndef	EXT_INTERRUPT_H_
#define EXT_INTERRUPT_H_


#include "../Common/Utils.h"
#include "ExtInterrupt_Cfg.h"



/*********************************************************************/

/* used in interrupt (global and local state) */
#define INTERRUPT_ENABLE	0b1
#define	INTERRUPT_DISABLE	0b0


/*********************************************************************/

/* use in ex0interrupt fire mode */
#define INTERRUPT0_LOW_LEVEL	0b00
#define INTERRUPT0_ANY_CHANGE	0b01
#define INTERRUPT0_FALLING_EDGE	0b10
#define INTERRUPT0_RISING_EDGE	0b11


/*********************************************************************/

/* use in ex1interrupt fire mode */
#define INTERRUPT1_LOW_LEVEL	0b00
#define INTERRUPT1_ANY_CHANGE	0b01
#define INTERRUPT1_FALLING_EDGE	0b10
#define INTERRUPT1_RISING_EDGE	0b11


/*********************************************************************/

/* use in ex2interrupt fire mode */
#define INTERRUPT2_FALLING_EDGE	0b0
#define INTERRUPT2_RISING_EDGE	0b1


/*********************************************************************/

StatusType interrupt_initialization (void); 			/* used to initialize the global and local enable of interrupts and in set their firemodes  */

StatusType interrupt_globalState( uint8 copy_u8state ); 	/* used in enable and disable of global enable of interrupts */


/*********************************************************************/

StatusType interrupt_ex0LocalState( uint8 copy_u8state);	/* used in enable and disable of local enable of external 0 interrupts */

StatusType interrupt_ex1LocalState( uint8 copy_u8state);	/* used in enable and disable of local enable of external 1 interrupts */

StatusType interrupt_ex2LocalState( uint8 copy_u8state);	/* used in enable and disable of local enable of external 2 interrupts */


/*********************************************************************/

void interrupt_ex0ClearFlag(void); 	/* used in clear flag of external 0 interrupts */

void interrupt_ex1ClearFlag(void);	/* used in clear flag of external 0 interrupts */

void interrupt_ex2ClearFlag(void);	/* used in clear flag of external 0 interrupts */


/*********************************************************************/

void interrupt_ex0ReadFlag(uint8 * copy_pu8Data) ;	/* used in read flag of external 0 interrupts */
void interrupt_ex1ReadFlag(uint8 * copy_pu8Data) ;	/* used in read flag of external 1 interrupts */
void interrupt_ex2ReadFlag(uint8 * copy_pu8Data) ;	/* used in read flag of external 2 interrupts */


/*********************************************************************/

StatusType interrupt_ex0FireMode (uint8 copy_u8FireMode);	/* used in set the fire mode of ex 0 interrupt */

StatusType interrupt_ex1FireMode (uint8 copy_u8FireMode);	/* used in set the fire mode of ex 1 interrupt */

StatusType interrupt_ex2FireMode (uint8);	/* used in set the fire mode of ex 2 interrupt */


/*********************************************************************/

void interrupt_external0 (void (* copy_ptr0Void_void )(void));	/* take the pointer of the ex0 interrupt function */

void interrupt_external1 (void (* copy_ptr0Void_void )(void));	/* take the pointer of the ex0 interrupt function */

void interrupt_external2 (void (* copy_ptr0Void_void )(void));	/* take the pointer of the ex0 interrupt function */


/*********************************************************************/



#endif	/* EXT_INTERRUPT_H_ */
