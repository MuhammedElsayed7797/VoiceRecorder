/*
 * Adc_Private.h
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 */

#ifndef ADC_ADC_PRIVATE_H_
#define ADC_ADC_PRIVATE_H_


/*****************************************************************************/


#define CONC_HELPER(A0,A1,A2,A3,A4,A5,A6,A7) 		0b##A7##A6##A5##A4##A3##A2##A1##A0
#define CONC(PA7,PA6,PA5,PA4,PA3,PA2,PA1,PA0) 		CONC_HELPER(PA7,PA6,PA5,PA4,PA3,PA2,PA1,PA0)


/*****************************************************************************/


#define ADC_ISR			__vector_14


/* the interrupt function depend on vector sent to it go back to vector table   */
#define	ISR( vector )			void vector (void) __attribute__ ((signal,used, externally_visible)) ; \
								void vector (void)


/*****************************************************************************/


#define ADMUX_REG	*( ( volatile uint8 * ) 0x27 )
#define SFIOR_REG	*( ( volatile uint8 * ) 0x50 )


/*****************************************************************************/


#endif /* ADC_ADC_PRIVATE_H_ */
