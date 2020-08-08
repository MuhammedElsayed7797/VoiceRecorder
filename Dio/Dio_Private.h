/*
 * Dio_Private.h
 *
 *  Created on: Jun 8, 2020
 *      Author: Bebo
 */

#ifndef DIO_DIO_PRIVATE_H_
#define DIO_DIO_PRIVATE_H_

/* used within Dio_Init to configure pin setting according to Dio_Cfg file */
#define CONC_HELP(A0,A1,A2,A3,A4,A5,A6,A7)		0b##A7##A6##A5##A4##A3##A2##A1##A0
#define CONC(PA7,PA6,PA5,PA4,PA3,PA2,PA1,PA0) 	CONC_HELP(PA7,PA6,PA5,PA4,PA3,PA2,PA1,PA0)

/* mask to clear interrupt bits used to set mode for INT0,1 */
#define DIO_INT0_CLEAR_MASK		0xFC
#define DIO_INT1_CLEAR_MASK		0xF3

#define DIO_INTERRUPT_ENABLE_CLEAR_MASK 	0x1F

/* mode set bits locatio for INT1,2 */
#define DIO_INT1_BIT_START		2
#define DIO_INT2_BIT_START		6

/* number of pins per port */
#define DIO_PINS_NUMBER			8

/* offset of pull up resistors disable bit */
#define DIO_PUD_BIT_OFFSET		2


#endif /* DIO_DIO_PRIVATE_H_ */
