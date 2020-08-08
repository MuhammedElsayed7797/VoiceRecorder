/*
 * ADC.h
 *
 *  Created on: Oct 5, 2019
 *      Author: DELL
 */

#ifndef ADC_H_
#define ADC_H_
#include "../std_types.h"
#include "../std_functions.h"
#include "../regs.h"


#define UNINIT -1

#define ADC_MODE_SINGLE 0
#define ADC_MODE_AUTOTRIGGER 1

#define ADC_REF_AVCC 0
#define ADC_REF_INTERNAL 1

#define ADC_CHANNEL_CH0 0
#define ADC_CHANNEL_CH1 1
#define ADC_CHANNEL_CH2 2
#define ADC_CHANNEL_CH3 3
#define ADC_CHANNEL_CH4 4
#define ADC_CHANNEL_CH5 5
#define ADC_CHANNEL_CH6 6
#define ADC_CHANNEL_CH7 7


#define ADC_AUTOTRIGGERSOURCE_FRUN 0
#define ADC_AUTOTRIGGERSOURCE_ACMP 1
#define ADC_AUTOTRIGGERSOURCE_EXINT0 2
#define ADC_AUTOTRIGGERSOURCE_TMR0CMP 3
#define ADC_AUTOTRIGGERSOURCE_TMR0OF 4
#define ADC_AUTOTRIGGERSOURCE_TMR1CMP 5
#define ADC_AUTOTRIGGERSOURCE_TMR1OF 6
#define ADC_AUTOTRIGGERSOURCE_TMR1CAPTURE 7

#define ADC_PRESCALAR_2 1
#define ADC_PRESCALAR_4 2
#define ADC_PRESCALAR_8 3
#define ADC_PRESCALAR_16 4
#define ADC_PRESCALAR_32 5
#define ADC_PRESCALAR_64 6
#define ADC_PRESCALAR_128 7

#define ADC_POLLING 0
#define ADC_INTERRUPT 1

#define ADC_BITS_8BIT 0
#define ADC_BITS_10BIT 1


void Adc_Set_CallBack(void(*Adc_ptr)(void));
void ADC_Init(void);
void ADC_StopConv(void);
void ADC_StartConv(U8 ADC_CHANNEL_CHx);



#endif /* ADC_H_ */
