/*
 * 	ExtInterrupt_Private.h
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 *
 */






#ifndef	EXT_INTERRUPT_PRIVATE_H_
#define EXT_INTERRUPT_PRIVATE_H_

/*********************************************************************/

#define SREG_INTERRUPT_BIT		7	/* reg's bit number that determine all interrupts  global enable bit */

/*********************************************************************/

#define GISR_INTERRUPT0_BIT 	6	/* reg's bit number that determine ex0  local enable bit */
#define GISR_INTERRUPT1_BIT 	7	/* reg's bit number that determine ex1  local enable bit */
#define GISR_INTERRUPT2_BIT 	5	/* reg's bit number that determine ex2  local enable bit */

/*********************************************************************/

#define GIFR_INTERRUPT0_BIT 	6	/* reg's bit number that determine ex0 flag bit */
#define GIFR_INTERRUPT1_BIT 	7	/* reg's bit number that determine ex1 flag bit */
#define GIFR_INTERRUPT2_BIT 	5	/* reg's bit number that determine ex2 flag bit */

/*********************************************************************/

#define MCUCR_INTERRUPT0_BIT0 	0	/* reg's bit number that determine ex0 fire mode */
#define MCUCR_INTERRUPT0_BIT1 	1	/* reg's bit number that determine ex0 fire mode */
#define MCUCR_INTERRUPT1_BIT0 	2	/* reg's bit number that determine ex1 fire mode */
#define MCUCR_INTERRUPT1_BIT1 	3	/* reg's bit number that determine ex1 fire mode */
#define MCUCSR_INTERRUPT2_BIT 	6	/* reg's bit number that determine ex2 fire mode */

/*********************************************************************/

#define CLEAR_FLAGE		1	/* the number which be write to flag reg to clear its determined bits */

/*********************************************************************/

#define SREG_regAdress 		0x5F 	/* its 7nth bit is global interrupt enable */
#define MCUCR_regAdress 	0x55 	/* reg used in determine fire mode of the interrupt 0 and 1  at bits 0:3 (int 1) , (int 0)*/
#define MCUCSR_regAdress	0x54 	/* reg used in determine fire mode of the interrupt 2  at bit 6 */
#define GISR_regAdress 		0x5B 	/* reg used in enable or disable the interrupt bits 7:5 (int1 , 0 ,2)  */
#define GIFR_regAdress 		0x5A 	/* reg of the flags of  interrupt bits 7:5 (int1 , 0 ,2) */

/*********************************************************************/

#define SREG 	*((volatile uint8 *) SREG_regAdress)
#define MCUCR 	*((volatile uint8 *) MCUCR_regAdress)
#define MCUCSR 	*((volatile uint8 *) MCUCSR_regAdress)
#define GISR 	*((volatile uint8 *) GISR_regAdress)
#define GIFR 	*((volatile uint8 *) GIFR_regAdress)

/*********************************************************************/

#define VECTOR_INT0		__vector_1	/* vector of ex0 interrupt */
#define VECTOR_INT1 	__vector_2	/* vector of ex1 interrupt */
#define VECTOR_INT2		__vector_18	/* vector of ex2 interrupt */

/*********************************************************************/

/* the interrupt function depend on vector sent to it go back to vector table   */
#define	private_exInterrupt(vector)		void vector (void) __attribute__ ((signal,used, externally_visible)) ; \
												void vector (void)

/*********************************************************************/

StatusType private_errorChecking (StatusType copy_validation);	/* help in determine validation of the function */

/*********************************************************************/

#endif	/* EXT_INTERRUPT_PRIVATE_H_ */
