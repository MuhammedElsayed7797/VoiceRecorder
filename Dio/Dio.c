/*
 * Dio.c
 *
 *  Created on: Jun 8, 2020
 *      Author: Bebo
 */

#include "../Dio/Dio.h"
#include "../Dio/Dio_Private.h"



/*****************************************************************************/


#if DIO_INIT_ON == TRUE

/* this function used to initialize all pins' direction and o/p state if (o/p) (tri-stated or pulled up) if input
 * according to configuration file Dio_Cfg.h
 *
 * used also to initialize state of interrupts (enabled or not) and its modes for INT0,1,2
 * according to configuration file Dio_Cfg.h
 *
 * avoid calling this function with enabled interrupt because this function change interrupt mode and if enabled
 * it might fire accidental interrupt
 *
 * this function clear external interrupts flag
 * */

void Dio_Init(void)
{
	/* initialize direction of all pins */
	*DDRA_REG = CONC
		(
			DIO_PIN0_DIRECTION,
			DIO_PIN1_DIRECTION,
			DIO_PIN2_DIRECTION,
			DIO_PIN3_DIRECTION,
			DIO_PIN4_DIRECTION,
			DIO_PIN5_DIRECTION,
			DIO_PIN6_DIRECTION,
			DIO_PIN7_DIRECTION
		) ;


	*DDRB_REG = CONC
		(
			DIO_PIN8_DIRECTION,
			DIO_PIN9_DIRECTION,
			DIO_PIN10_DIRECTION,
			DIO_PIN11_DIRECTION,
			DIO_PIN12_DIRECTION,
			DIO_PIN13_DIRECTION,
			DIO_PIN14_DIRECTION,
			DIO_PIN15_DIRECTION
		) ;


	*DDRC_REG = CONC
	(
			DIO_PIN16_DIRECTION,
			DIO_PIN17_DIRECTION,
			DIO_PIN18_DIRECTION,
			DIO_PIN19_DIRECTION,
			DIO_PIN20_DIRECTION,
			DIO_PIN21_DIRECTION,
			DIO_PIN22_DIRECTION,
			DIO_PIN23_DIRECTION
		) ;


	*DDRD_REG = CONC
		(
			DIO_PIN24_DIRECTION,
			DIO_PIN25_DIRECTION,
			DIO_PIN26_DIRECTION,
			DIO_PIN27_DIRECTION,
			DIO_PIN28_DIRECTION,
			DIO_PIN29_DIRECTION,
			DIO_PIN30_DIRECTION,
			DIO_PIN31_DIRECTION
		) ;

	/*****************************/

	/* initialize state of all pins */
	*PORTA_REG = CONC
		(
			DIO_PIN0_STATE,
			DIO_PIN1_STATE,
			DIO_PIN2_STATE,
			DIO_PIN3_STATE,
			DIO_PIN4_STATE,
			DIO_PIN5_STATE,
			DIO_PIN6_STATE,
			DIO_PIN7_STATE
		) ;


	*PORTB_REG = CONC
		(
			DIO_PIN8_STATE,
			DIO_PIN9_STATE,
			DIO_PIN10_STATE,
			DIO_PIN11_STATE,
			DIO_PIN12_STATE,
			DIO_PIN13_STATE,
			DIO_PIN14_STATE,
			DIO_PIN15_STATE
		) ;



	*PORTC_REG = CONC
		(
			DIO_PIN16_STATE,
			DIO_PIN17_STATE,
			DIO_PIN18_STATE,
			DIO_PIN19_STATE,
			DIO_PIN20_STATE,
			DIO_PIN21_STATE,
			DIO_PIN22_STATE,
			DIO_PIN23_STATE
		) ;



	*PORTD_REG = CONC
		(
			DIO_PIN24_STATE,
			DIO_PIN25_STATE,
			DIO_PIN26_STATE,
			DIO_PIN27_STATE,
			DIO_PIN28_STATE,
			DIO_PIN29_STATE,
			DIO_PIN30_STATE,
			DIO_PIN31_STATE
		) ;

	/*****************************/
	/* clear interrupt flags */

	*GIFR_REG |= 0x00 ;

	/*****************************/

	/* initialize state of interrupts and their modes */
	*GICR_REG &= DIO_INTERRUPT_ENABLE_CLEAR_MASK ;
	*GICR_REG |= CONC( DIO_INT1_ENABLE, DIO_INT0_ENABLE, DIO_INT2_ENABLE, 0, 0, 0, 0, 0 ) ;


	*MCUCR_REG &= DIO_INT0_CLEAR_MASK ;
	*MCUCR_REG |= DIO_INT0_MODE ;

	*MCUCR_REG &= DIO_INT1_CLEAR_MASK ;
	*MCUCR_REG |= ( DIO_INT0_MODE << DIO_INT1_BIT_START ) ;

	ASSIGN_BIT( *MCUCSR_REG, DIO_INT2_BIT_START, (DIO_INT2_MODE & 0x01) ) ;

	/*****************************/

	/* configure global pull up resistors disable*/
	SFIOR_REG_BIT->Bit_2 = DIO_PULL_UP_GLOBAL_DISABLE ;



	return ;
}

#endif

/*****************************************************************************/


#if DIO_INIT_ON == TRUE

/* this if pin is o/p function used to determine o/p level on pin which its number represented by PinID
 * and level represented by State if equal DIO_HIGH o/p will equal VCC, if DIO_LOW o/p will equal GND
 *
 * if pin is i/p function used to enable/disable internal pull up resistor for pin number represented by PinID
 * if State equal DIO_HIGH pull up resistor will enabled locally
 * if State equal DIO_LOW pull up resistor will disabled locally and pin will be tri-stated input
 *
 * note: if pin is represented as o/p and has high level then reconfigured as input then
 * if pull up resistors globally, pin will be pulled up input
 * and if was low o/p and reconfigured as input it will be tri-stated input
 *
 * */
void Dio_WritePin ( uint8 PinID, Dio_PinLevel State )
{
	uint8 PinNumber = PinID % DIO_PINS_NUMBER , PortNumber = PinID / DIO_PINS_NUMBER ;

	ASSIGN_BIT( *( PORTA_REG + ( 3 * PortNumber ) ), PinNumber, State ) ;

	return ;
}

#endif

/*****************************************************************************/

#if DIO_INIT_ON == TRUE

/*
 * this function read level on pin which its number represented by PinID
 * if pin is input  function will return external voltage on this pin
 *
 * if output function will return output level of this pin (read back)
 *
 * if pin state change it will delay to change inside AVR registers and returned by this function
 * the changed level need  0.5 -> 1.5 clock cycle if it was external state,
 * and to 1 clock cycle if pin is output and want to read back it's value
 *
 * */

Dio_PinLevel Dio_ReadPin ( uint8 PinID )
{
	uint8 PinNumber = PinID % DIO_PINS_NUMBER , PortNumber = PinID / DIO_PINS_NUMBER ;

	return GET_BIT( *( PINA_REG + ( 3 * PortNumber ) ), PinNumber ) ;

}

#endif

/*****************************************************************************/

#if DIO_INIT_ON == TRUE

/*
 * this function will toggle level of pin which number represented by PinID if o/p
 * if input then it will toggle between pulled up input and tri-stated input
 *
 * */

void Dio_FlipPin ( uint8 PinID )
{
	uint8 PinNumber = PinID % DIO_PINS_NUMBER , PortNumber = PinID / DIO_PINS_NUMBER ;
	TOG_BIT( *( PORTA_REG + ( 3 * PortNumber ) ), PinNumber ) ;

	return ;
}

#endif

/*****************************************************************************/

#if DIO_INIT_ON == TRUE

/*
 * this function like Dio_WritePin function put for entire port which its number represented by PortID
 * State bits represent state of every pin in the port MSB of state represent highest pin in the port
 * if bit = 0 corresponding pin will = DIO_LOW and if bit = 1 pin will = DIO_HIGH
 *
 * */
void Dio_WritePort ( uint8 PortID, Dio_PortLevel State )
{

	ASSIGN_PORT( *( PORTA_REG + ( 3 * PortID ) ), State ) ;

	return ;
}

#endif

/*****************************************************************************/



#if DIO_INIT_ON == TRUE

/*
 * this function like Dio_ReadPin function put for entire port which its number represented by PortID
 * MSB of return value will represent level on highest pin in the port
 *
 * */
Dio_PortLevel Dio_ReadPort ( uint8 PortID )
{
	return *( PINA_REG + ( 3 * PortID ) ) ;
}

#endif

/*****************************************************************************/


#if DIO_INIT_ON == TRUE



/*
 * this function like Dio_FlipPin function put for entire port which its number represented by PortID
 *
 * */
void Dio_FlipPort ( uint8 PortID )
{
	*( PORTA_REG + ( 3 * PortID ) ) ^= 0xFF ;
	return ;
}

#endif

/*****************************************************************************/


#if DIO_INIT_ON == TRUE

/*
 * this function like Dio_WritePort but with input Mask
 * to select specific pins to writes their states if pin's bit in Mask is 0
 * then it's state won't be changed according to State variable
 *
 * */

void Dio_WriteGroup( uint8 PortID, Dio_PortLevel State, Dio_PortLevel Mask )
{
	State &= Mask ;
	State |= (~Mask) & (*( PORTA_REG + ( 3 * PortID ) )) ;
	ASSIGN_PORT( *( PORTA_REG + ( 3 * PortID ) ), State ) ;

	return ;
}

#endif

/*****************************************************************************/



#if DIO_INIT_ON == TRUE


/*
 * this function like Dio_ReadPort but with input Mask
 * to select specific pins to read their states if pin's bit in Mask is 0
 * then it's corresponding bit in return value will be 0 regard less of actual state of pin
 *
 * */
Dio_PortLevel Dio_ReadGroup ( uint8 PortID, Dio_PortLevel Mask )
{

	return (*( PINA_REG + ( 3 * PortID ) )) & Mask ;
}

#endif

/*****************************************************************************/


#if DIO_INIT_ON == TRUE


/*
 * this function like Dio_FlipPort but with input Mask
 * to select specific pins to flip their states if pin's bit in Mask is 0
 * then it's state won't be changed
 *
 * */

void Dio_FlipGroup ( uint8 PortID, Dio_PortLevel Mask )
{
	*( PORTA_REG + ( 3 * PortID ) ) ^= Mask ;
	return ;
}

#endif

/*****************************************************************************/


#if DIO_INIT_ON == TRUE

/*
 * this function used to set direction of pin which its number represented by PinID
 * to be either input if Direction = DIO_INPUT or to be output if Direction = DIO_OUTPUT
 * */
void Dio_SetPinDirection(  uint8 PinID, Dio_PinDirection Direction )
{
	uint8 PinNumber = PinID % DIO_PINS_NUMBER , PortNumber = PinID / DIO_PINS_NUMBER ;

	ASSIGN_BIT( *( DDRA_REG + ( 3 * PortNumber ) ), PinNumber, Direction ) ;

	return ;
}

#endif

/*****************************************************************************/


#if DIO_INIT_ON == TRUE

/*
 * this function is like Dio_SetPinDirection but for entire port
 * MSB of Direction represent highest pin direction if 1 then pin is output
 * if 0 then pin is input */

void Dio_SetPortDirection(  uint8 PortID, Dio_PortDirection Direction )
{
	ASSIGN_PORT( *( DDRA_REG + ( 3 * PortID ) ), Direction ) ;

	return ;
}

#endif

/*****************************************************************************/


#if DIO_INIT_ON == TRUE

/*
 * this function is like Dio_SetPortDirection but wiht mask to select pins to change their direction
 * if pin bit in Mask = 0 then its direction won't be changed according to Direction
 *
 * */

void Dio_SetGroupDirection(  uint8 PortID, Dio_PortDirection Direction,  Dio_PortLevel Mask )
{

	Direction &= Mask ;
	Direction |= (~Mask) & (*( PORTA_REG + ( 3 * PortID ) )) ;
	ASSIGN_PORT( *( DDRA_REG + ( 3 * PortID ) ), Direction ) ;

	return ;
}


#endif

/*****************************************************************************/

#if DIO_INIT_ON == TRUE

/*
 * this function used to set interrupt trigger setting
 * InterruptID represent interrupt number
 * Mode represent mode to set for interrupt
 *
 * in case of interrupt 2 just falling, rising edge is allowed
 *
 * when changing interrupt mode it might fire an interrupt if enabled
 * so to avoid accidental  before change mode , disable interrupt then change mode then clear interrupt flag
 * and then enable interrupt again
 *
 * */
void Dio_SetInterruptMode ( Dio_InterruptType InterruptID, Dio_InterruptMode  Mode )
{
	switch (InterruptID)
	{
		case DIO_INT2 :
		{

			/* INT2 just need  1 bit for setting INT2 mode */
			Mode &= 0x01 ;
			ASSIGN_BIT( *MCUCSR_REG, DIO_INT2_BIT_START, Mode) ;

			break ;
		}

		case DIO_INT0 :
		{

			*MCUCR_REG &= DIO_INT0_CLEAR_MASK ;
			*MCUCR_REG |= Mode ;
			break ;
		}

		case DIO_INT1 :
		{
			*MCUCR_REG &= DIO_INT1_CLEAR_MASK ;
			*MCUCR_REG |= Mode << DIO_INT1_BIT_START ;

			break ;
		}
	}
	return ;
}

#endif

/*****************************************************************************/



