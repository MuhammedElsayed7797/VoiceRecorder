
/*
 * TIM.c
 *
 *  Created on: Dec 17, 2019
 *      Author: DELL
 */
#include "Timer.h"


/* interrupt function depend on vector sent to it go back to vector table   */
#define	ISR( vector )			void vector (void) __attribute__ ((signal,used, externally_visible)) ; \
								void vector (void)
								
								

static void (*TIM0_COMP_ptr)(void);
static void (*TIM2_COMP_ptr)(void);
static void (*TIM2_OVF_ptr)(void);



void TIM0_Init(void){
	TIMSK = (TIMSK & (~0x3)) | (2 << 0);
	setbit(SREG,7);
}

void TIM0_Start(void){
	/* Match every 125micro seconds */
	setbit(TCCR0,3);
	clrbit(TCCR0,6);
	TCCR0 = (TCCR0 & (~0x07)) | (3 << 0);   /* Prescaler = 64 */
	OCR0 = 0x1F;
}

void TIM0_Stop(void){
	TCCR0 = (TCCR0 & (~0x07)) | (0 << 0);
}

void TIM0_COMP_SetISR(void (*ptr)(void)){
	TIM0_COMP_ptr = ptr;
}

void TIM0_SetTimeMS(U32 time_ms){
	OCR0 = (time_ms * 1000) % 256;
}

void TIM2_Init(void){
	//Fast PWM
	setbit(TCCR2,3);
	setbit(TCCR2,6);

	//Global Interrupt
	//SetBit(SREG,7);

	//OC2 Pin (non-inverting)
	setbit(TCCR2,5);

	//Interrupt Enable
	clrbit(TIMSK,6);
	clrbit(TIMSK,7);


}

void TIM2_SetDutyCycle(U8 DC)
{
	OCR2 = DC;
	return ;
}

void TIM2_COMP_SetISR(void (*ptr)(void)){
	TIM2_COMP_ptr = ptr;
}

void TIM2_OVF_SetISR(void (*ptr)(void)){
	TIM2_OVF_ptr = ptr;
}

void TIM2_Start()
{
	TCNT2 = 0x00;
	TCCR2 = (TCCR2 & (~0x07)) | (1 << 0);
	

}
void TIM2_Stop()
{
	TCCR2 = (TCCR2 & (~0x07)) | (0 << 0);	
}

ISR(__vector_4){
	(TIM2_OVF_ptr)();
}

ISR(__vector_3){
	(TIM2_COMP_ptr)();
}
ISR(__vector_19)
{
	TIM0_COMP_ptr() ;
}
ISR(__vector_9){
	/*count++;
	if (count % 3 == 0) togglebit(PORTB,0);	*/
}
