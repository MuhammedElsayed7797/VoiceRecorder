/*
 *  main.c
 *
 *  Created on: Mar 22, 2020
 *  Author: Bebo
 */



/***********************************************************************************/

#include "../Common/Utils.h"
#include "../DIO/DIO_int.h"
#include "../ADC/ADC_int.h"
#include "../TIMER/Timer.h"
#include "../ExtInterrupt/ExtInterrupt.h"
#include "../TWI/TWI.h"
#include "../EEPROM_24C512/EEPROM_24C512.h"


/***********************************************************************************/

/* NOTE : for EEPROM 24C512 it max write operation could be done is page write
 * which is 128 byte write so buffer size is 130 according to READ and WRITE functions
 * of EEPROM
 *
 * code written is just for 2 buffers if more is needed then VRec_ADC_ISR function
 * must be modified in toggle of buffers part
 * */

#define VREC_EEPROM_START_BYTE_ADDRESS	0x0000
#define VREC_EEPROMS_NUMBER		2
#define VREC_EEPROM_PAGE_SIZE	0x080u

/* number of buffers and size of each buffer */
#define VREC_BUFFERS_NUMBER		2
#define VREC_BUFFERS_SIZE		130

/* pins used as buttons to play and record operations*/
#define VREC_RECORD_BUTTON		25
#define VREC_PLAY_BUTTON		26

/* pins used as leds to play and record operations*/
#define VREC_RECORD_LED			1
#define VREC_PLAY_LED			2

/* channel of ADC, PWM used */
#define VREC_INPUT_CHANNEL		ADC_CHANNEL_CH0
#define VREC_OUTPUT_CHANNEL

/* initial values for some variables
 * its initiated with values to make overflow when first call to ADC conversion is happened
 *
 * VREC_ADC_BUFFER_INDEX_INITIAL is 1 due to first 2 bytes in buffer will be overrided by EEPROM write and read functions
 * */
#define	VREC_ADC_BUFFER_INDEX_INITIAL			0x01u
#define	VREC_STOP_INDICATOR_RECORD_INITIAL		0xFFu
#define	VREC_EEPROM_INTERNAL_ADDRESS_INITIAL	0xFF80u

#define	VREC_PWM_BUFFER_INDEX_INITIAL			0x00u
#define VREC_STOP_INDICATOR_PLAY_INITIAL		0x00u
#define VREC_EEPROM_NUMBER_PLAY_INITIAL			0x00u

/***********************************************************************************/

void VRec_RecordVoice (void) ;
void VRec_PlayVoice (void) ;
void VRec_Timer0_ISR (void) ;
void VRec_ADC_ISR (void) ;


/***********************************************************************************/

/* flag to determine code entered in timer 0 ISR */
boolean VRec_PWM_Flag	= FALSE ;

/* buffers for data of ADC and EEPROM */
uint8 VRec_Buffer_Array [VREC_BUFFERS_NUMBER][VREC_BUFFERS_SIZE] = {{0,0,VAL_128(0x33)},{0,0,VAL_128(0x33)}};

/* temporary pointer buffer used when toggle ADC and EEPROM buffers */
uint8 * VRec_Temp_Buffer_Address = NULL_PTR ;

/* pointer of ADC buffer */
uint8 * VRec_ADC_PWM_Buffer_Address = VRec_Buffer_Array[0] ;

/* pointer of EEPROM buffer */
uint8 * VRec_EEPROM_Buffer_Address = VRec_Buffer_Array[1] ;

/* indication to EEPROM which will be writed */
uint8 VRec_EEPROM_Number = VREC_EEPROMS_NUMBER - 1 ;

/* index of ADC buffer */
uint8 VRec_Buffer_Index = VREC_ADC_BUFFER_INDEX_INITIAL ;

/* used to stop record when all EEPROM was written */
uint8 VRec_Stop_Indicator = VREC_STOP_INDICATOR_RECORD_INITIAL ;

/* used to carry address of byte which will be written inside EEPROM*/
uint16 VRec_EEPROM_Internal_Address	= VREC_EEPROM_INTERNAL_ADDRESS_INITIAL ;

/*
uint8 dac_1 [3] = {0x30,0xFF,0xFF};
uint8 dac_2 [3] = {0x00,0xFF,0x00};
*/

/***********************************************************************************/

void hello (char * string)
{
	*string = 0 ;
	return ;
}

int main (void)
{
	hello("test string");
	
	
	__asm(" MOVW R26, R0");
	__asm(" LDI R16, 0x10");
	__asm(" ST X, r16");
	
	

	/* global interrupt enable*/

	/* initialize direction, internal resistors , initial value
	 * of  DIO, ADC, interrupts, PWM, TWI */
	DIO_Init () ;

	/* initialize INT 0,1 firing mode */
	interrupt_initialization () ;

	/* initialize TWI bit rate and scale, and notifications to be triggered to use EEPROM */
	TWI_Init () ;

	/*
	test I2C DAC 
	interrupt_globalState (INTERRUPT_ENABLE) ;

	while ( E_NOT_OK == TWI_MT(dac_1,0x30u,3) ) ;
	while ( E_NOT_OK == TWI_MT(dac_2,0x30u,3) ) ;

	
	while(1)
	{
		__asm("NOP");
	}
	**/	
	/* initialize Timer 0 to trigger every 125us to start ADC conversion */
	TIM0_Init () ;

	/* initialize Timer 1 to make PWM o/p */
	TIM2_Init () ;

	/* initialize ADC to auto conversion according to TIMER 0 trigger and to 8 bit resolution */
	ADC_Init () ;

	/* set callback functions for timer0 ,ADC , INT0, INT1 */

	interrupt_external0 (VRec_RecordVoice) ;

	interrupt_external2 (VRec_PlayVoice) ;

	Adc_Set_CallBack(VRec_ADC_ISR) ;

	TIM0_COMP_SetISR (VRec_Timer0_ISR) ;

	/* global interrupt enable*/
	interrupt_globalState (INTERRUPT_ENABLE) ;

	while (1)
	{
		__asm("NOP") ;
	}
	return 0 ;
}



/***********************************************************************************/

/* function called to start record */
void VRec_RecordVoice (void)
{

	interrupt_ex0ClearFlag() ;
	DIO_SetPinValue ( VREC_RECORD_LED, 0 ) ;

	/* stop timer in case of multiple clicking on record switch */
	TIM0_Stop() ;

	/* initiate ADC, EEPROM buffer, and variables */
	
	VRec_Buffer_Index =	VREC_ADC_BUFFER_INDEX_INITIAL ;

	VRec_Stop_Indicator = VREC_STOP_INDICATOR_RECORD_INITIAL ;

	VRec_EEPROM_Internal_Address = VREC_EEPROM_INTERNAL_ADDRESS_INITIAL ;

	VRec_ADC_PWM_Buffer_Address = VRec_Buffer_Array [0] ;

	VRec_EEPROM_Buffer_Address = VRec_Buffer_Array [1] ;


	/* start ADC conversion on Channel 0 */
	ADC_StartConv(VREC_INPUT_CHANNEL) ;


	/* start timer 0 counter to trigger ADC conversion */
	TIM0_Start() ;

	DIO_SetPinValue ( VREC_RECORD_LED, 1 ) ;



	return ;
}


/***********************************************************************************/

/* function called to play voice inside EEPROM*/
void VRec_PlayVoice (void)
{

	uint8 LocalCounter = 0x00u ;

	DIO_SetPinValue ( VREC_PLAY_LED, 0 ) ;


	interrupt_ex2ClearFlag() ;
	interrupt_globalState (INTERRUPT_ENABLE) ;

	/* stop timer in case of multiple clicking on record switch */
	TIM0_Stop() ;
	ADC_StopConv () ;

	/* set PWM flag to enter PWM code inside timer 0 ISR */
	VRec_PWM_Flag = TRUE ;

	/* initiate PWM, EEPROM buffer, and variables */

	VRec_Buffer_Index =	VREC_PWM_BUFFER_INDEX_INITIAL ;

	VRec_Stop_Indicator = VREC_STOP_INDICATOR_PLAY_INITIAL ;

	VRec_EEPROM_Internal_Address = VREC_EEPROM_INTERNAL_ADDRESS_INITIAL + VREC_EEPROM_PAGE_SIZE ;

	VRec_EEPROM_Buffer_Address = VRec_Buffer_Array [0] ;

	VRec_ADC_PWM_Buffer_Address = VRec_Buffer_Array [0] ;

	VRec_EEPROM_Number = VREC_EEPROM_NUMBER_PLAY_INITIAL ;


	/* write internal address of EEPROMs to 0X00 */
	for( LocalCounter = 0 ; LocalCounter < VREC_EEPROMS_NUMBER ; LocalCounter++ )
	{
		while ( E_NOT_OK == EE24C512_Write( LocalCounter, &VRec_EEPROM_Internal_Address, VRec_EEPROM_Buffer_Address, 0 ) ) ;
	}

	/* initialize both buffers with EEPROM context*/
	while ( E_NOT_OK == EE24C512_Read( VRec_EEPROM_Number, NULL_PTR, VRec_EEPROM_Buffer_Address, VREC_BUFFERS_SIZE-2 ) ) ;

	/* increment EEPROM internal address and initialize other buffer */
	VRec_EEPROM_Internal_Address += VREC_EEPROM_PAGE_SIZE ;
	VRec_EEPROM_Buffer_Address = VRec_Buffer_Array [1] ;

	while ( E_NOT_OK == EE24C512_Read( VRec_EEPROM_Number, NULL_PTR, VRec_EEPROM_Buffer_Address, VREC_BUFFERS_SIZE-2 ) ) ;

	
	while ( TRUE == EE24C512_BusyFlag) 
	{
		__asm("NOP");
	}

	/* start timer 0 counter to trigger ADC conversion */
	TIM0_Start() ;
	TIM2_Start() ;


	DIO_SetPinValue ( VREC_PLAY_LED, 1 ) ;


	return ;
}


/***********************************************************************************/

void VRec_Timer0_ISR (void)
{
	if ( TRUE == VRec_PWM_Flag )
	{
		/* write analog value using PWM signal */
		TIM2_SetDutyCycle( ( (VRec_ADC_PWM_Buffer_Address [ VRec_Buffer_Index ++ ]) << 1 ) );


		/* case all buffer was read and output by PWM  */
		if( (VREC_BUFFERS_SIZE - 2) == VRec_Buffer_Index)
		{
			/* modify address of EEPROM to start read from */
			VRec_EEPROM_Internal_Address += VREC_EEPROM_PAGE_SIZE ;

			/* go to next EEPROM if previous was read  */
			if( VREC_EEPROM_START_BYTE_ADDRESS == VRec_EEPROM_Internal_Address)
			{
				VRec_Stop_Indicator ++ ;
				VRec_EEPROM_Number =  ( VRec_EEPROM_Number + 1 ) % VREC_EEPROMS_NUMBER ;
			}
			else
			{

			}

			/* toggle buffers between PWM and EEPROM and initiate Index */
			VRec_Temp_Buffer_Address = VRec_ADC_PWM_Buffer_Address ;
			VRec_ADC_PWM_Buffer_Address = VRec_EEPROM_Buffer_Address ;
			VRec_EEPROM_Buffer_Address = VRec_Temp_Buffer_Address ;

			VRec_Buffer_Index = VREC_PWM_BUFFER_INDEX_INITIAL ;

			/* check if both EEPROM was read*/
			if( VRec_Stop_Indicator < VREC_EEPROMS_NUMBER )
			{
				/* read from EEPROM */
				while ( E_NOT_OK == EE24C512_Read( VRec_EEPROM_Number, NULL_PTR, VRec_EEPROM_Buffer_Address, VREC_BUFFERS_SIZE-2 ) ) ;
			}
			else
			{
				DIO_SetPinValue ( VREC_PLAY_LED, 0 ) ;
				TIM0_Stop() ;
				VRec_PWM_Flag	= FALSE ;

				/*  disable interrupt */
			}

		}
		else
		{

		}
	}
	else
	{
		/* just trigger ADC conversion */
	}


	return ;
}

/***********************************************************************************/

/* used to save ADC read and call EEPROM write when ADC buffer is full */
void VRec_ADC_ISR (void)
{
	/* read conversion result into ADC buffer */
	VRec_ADC_PWM_Buffer_Address [ VRec_Buffer_Index ++ ] = ( ( ADCH >> 1 ) | 0x80u );

	/* case buffer is full */
	if( VREC_BUFFERS_SIZE == VRec_Buffer_Index)
	{
		/* modify address of EEPROM to start write in */
		VRec_EEPROM_Internal_Address += VREC_EEPROM_PAGE_SIZE ;

		/* go to next EEPROM if previous is full */
		if( VREC_EEPROM_START_BYTE_ADDRESS == VRec_EEPROM_Internal_Address)
		{
			VRec_Stop_Indicator ++ ;
			VRec_EEPROM_Number =  ( VRec_EEPROM_Number + 1 ) % VREC_EEPROMS_NUMBER ;
		}
		else
		{

		}

		/* toggle buffers between ADC and EEPROM and initiate Index */
		VRec_Temp_Buffer_Address = VRec_ADC_PWM_Buffer_Address ;
		VRec_ADC_PWM_Buffer_Address = VRec_EEPROM_Buffer_Address ;
		VRec_EEPROM_Buffer_Address = VRec_Temp_Buffer_Address ;

		VRec_Buffer_Index = VREC_ADC_BUFFER_INDEX_INITIAL ;

		/* check if both EEPROM was written*/
		if( VRec_Stop_Indicator < VREC_EEPROMS_NUMBER )
		{
			/* write to EEPROM */
			while ( E_NOT_OK == EE24C512_Write( VRec_EEPROM_Number, &VRec_EEPROM_Internal_Address, VRec_EEPROM_Buffer_Address, VREC_BUFFERS_SIZE-2 ) ) ;
		}
		else
		{
			DIO_SetPinValue ( VREC_RECORD_LED, 0 ) ;
			TIM0_Stop() ;

			/* stop conversion */
			ADC_StopConv () ;
		}

	}
	else
	{

	}

	return ;
}

/***********************************************************************************/
