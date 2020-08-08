/*
 * 	ExtInterrupt_Cfg.h
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 *
 */


#ifndef	EXT_INTERRUPT_CFG_H_
#define EXT_INTERRUPT_CFG_H_

/*********************************************************************/

/* set the state of global interrupts enable mode  */
#define GLOBAL_INTERRUPT_STATE	INTERRUPT_DISABLE

/*********************************************************************/

/* set the state of local interrupts enable mode  */
#define INTERRUPT0_FIRE_MODE	INTERRUPT0_RISING_EDGE
#define INTERRUPT1_FIRE_MODE	INTERRUPT1_RISING_EDGE
#define INTERRUPT2_FIRE_MODE	INTERRUPT2_RISING_EDGE

/*********************************************************************/

/* set the state of fire mode of interrupts  */
#define INTERRUPT0_STATE		INTERRUPT_ENABLE
#define INTERRUPT1_STATE		INTERRUPT_DISABLE
#define INTERRUPT2_STATE		INTERRUPT_DISABLE

/*********************************************************************/

#endif	/* EXT_INTERRUPT_CFG_H_ */
