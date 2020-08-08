// This file should only contains function prototypes 

#ifndef DIO_int
#define DIO_int
#include "../std_types.h"
#include "../regs.h"
#include "DIO_priv.h"
#include "DIO_cfg.h"
#include "../std_functions.h"

#define OUT 1
#define HIGH 1
#define LOW 0
void DIO_Init(void);
void DIO_SetPinDir(U8 copy_U8_PinNum,U8 copy_U8_PinDir);
void DIO_SetPinValue(U8 copy_U8_PinNum,U8 copy_U8_Pinvalue);
U8 DIO_GetPinValue(U8 copy_U8_PinNum);


#endif
