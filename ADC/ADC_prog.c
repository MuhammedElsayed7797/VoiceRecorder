/*
 * ADC.c
 *
 *  Created on: Oct 5, 2019
 *      Author: DELL
 */
 
#include "ADC_int.h"


/* the interrupt function depend on vector sent to it go back to vector table   */
#define	ISR(vector)				void vector (void) __attribute__ ((signal,used, externally_visible)) ; \
								void vector (void)
								
		
static void (*ptr_AdcInt)(void) = NULL;


void ADC_Init(void){
	setbit(ADCSRA,7);     /* Voltage reference and input channel selection will not go into effect until ADEN is set */
}


void ADC_StartConv(U8 ADC_CHANNEL_CHx){
#if ADC_MODE == ADC_MODE_SINGLE
	clrbit(ADCSRA,5);
	setbit(ADCSRA,3);
#elif (ADC_MODE == ADC_MODE_AUTOTRIGGER && ADC_AUTOTRIGGERSOURCE == ADC_AUTOTRIGGERSOURCE_TMR0CMP)
	setbit(ADCSRA,5);
	SFIOR = (SFIOR & (~0xE0)) | (ADC_AUTOTRIGGERSOURCE_TMR0CMP << 5);
	setbit(ADCSRA,3);
#endif

	ADCSRA = (ADCSRA & (~0x07)) | (1 << 0);
#if ADC_BITS == ADC_BITS_8BIT
	setbit(ADMUX,5);
#elif ADC_BITS == ADC_BITS_10BIT
	clrbit(ADMUX,5);
#endif

#if ADC_REF == ADC_REF_AVCC
	setbit(ADMUX,6);
	clrbit(ADMUX,7);
#elif ADC_REF == ADC_REF_INTERNAL
	setbit(ADMUX,6);
	setbit(ADMUX,7);
#endif

	switch(ADC_CHANNEL_CHx){
	case ADC_CHANNEL_CH0:
		ADMUX = (ADMUX & (~0x1F)) | (ADC_CHANNEL_CH0 << 0);
		break;
	case ADC_CHANNEL_CH1:
		ADMUX = (ADMUX & (~0x1F)) | (ADC_CHANNEL_CH1 << 0);
		break;
	case ADC_CHANNEL_CH2:
		ADMUX = (ADMUX & (~0x1F)) | (ADC_CHANNEL_CH2 << 0);
		break;
	case ADC_CHANNEL_CH3:
		ADMUX = (ADMUX & (~0x1F)) | (ADC_CHANNEL_CH3 << 0);
		break;
	case ADC_CHANNEL_CH4:
		ADMUX = (ADMUX & (~0x1F)) | (ADC_CHANNEL_CH4 << 0);
		break;
	case ADC_CHANNEL_CH5:
		ADMUX = (ADMUX & (~0x1F)) | (ADC_CHANNEL_CH5 << 0);
		break;
	case ADC_CHANNEL_CH6:
		ADMUX = (ADMUX & (~0x1F)) | (ADC_CHANNEL_CH6 << 0);
		break;
	case ADC_CHANNEL_CH7:
		ADMUX = (ADMUX & (~0x1F)) | (ADC_CHANNEL_CH7 << 0);
		break;
	}
	setbit(ADCSRA,6);
	
}

void Adc_Set_CallBack(void(*Adc_ptr)(void))
{
	ptr_AdcInt = Adc_ptr;
}
void ADC_StopConv(void)
{
	clrbit(ADCSRA,7);
}
ISR(__vector_14){
	
	if (NULL != ptr_AdcInt)
		ptr_AdcInt();
	else
	{
		/* Do nothing */
	}
}


