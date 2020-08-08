/*
 * TIM.h
 *
 *  Created on: Dec 17, 2019
 *      Author: DELL
 */

#ifndef TIM_H_
#define TIM_H_

#include "../std_types.h"
#include "../std_functions.h"
#include "../regs.h"
#define Req_Freq 800
#define Clk_Freq 16000000

void TIM0_Init(void);
void TIM0_Start(void);
void TIM0_Stop(void);

void TIM0_COMP_SetISR(void (*ptr)(void));
void TIM0_SetTimeMS(U32 time_ms);

void TIM2_Init(void);
void TIM2_Stop(void);
void TIM2_Start(void);
void TIM2_SetDutyCycle(U8 DC);

void TIM2_COMP_SetISR(void (*ptr)(void));
void TIM2_OVF_SetISR(void (*ptr)(void));

#endif /* TIM_H_ */
