/*
 * Dio.h
 *
 *  Created on: Jun 8, 2020
 *      Author: Bebo
 */

#ifndef DIO_DIO_H_
#define DIO_DIO_H_


#include "../Common/Utils.h"
#include "../Dio/Dio_Cfg.h"


/*****************************************************************************/

/* used to represent PIN state
 * if pin is output HIGH->VCC, LOW-> GND
 * if pin is input HIGH->pulled up input
 * LOW->tri-stated input
 *
 * */

#define DIO_LOW		0
#define DIO_HIGH	1
typedef boolean Dio_PinLevel ;


/* used to represent pin direction
 *  DIO_INPUT->pin will be input
 *  DIO_INPUT->pin will be o/p
 *
 *  input functionality of pin will be in both input and output pin
 *  for input pin it will read external level on pin
 *  for output pin it will read o/p level of pin (read back)
 *
 *  */

#define DIO_INPUT	0
#define DIO_OUTPUT	1
typedef boolean Dio_PinDirection ;


/*****************************************************************************/

/* used to as Dio_PinLevel but for all port every bit in Dio_PortLevel variable
 * will represent an pin in this port if bit = 0 then pin will be as DIO_LOW
 * MSB will represent highest pin in the port
 *
 * */

typedef uint8 Dio_PortLevel ;

/* used to as Dio_PinDirection but for all port every bit in Dio_PortDirection variable
 * will represent an pin in this port if bit = 0 then pin will be as DIO_INPUT
 * MSB will represent highest pin in the port
 *
 * */
typedef uint8 Dio_PortDirection ;


/* used to represent external interrupts
 * DIO_INT0,1,2 for external interrupt 0,1,2
 *
 * */

#define DIO_INT0	1
#define DIO_INT1	2
#define DIO_INT2	0
typedef uint8 Dio_InterruptType ;

/*****************************************************************************/

/*
 * software interrupt could be generated when Interrupt pin is configured as output and interrupt is enabled
 * and using software make output sufficient to generate interrupt according to interrupt mode
 *
 * like in case of rising edge o/p will make LOW for 1 clock cycle then high for 1 clock cycle
 *
 * */


/* used to represent interrupt mode
 * for external interrupt 0,1 all modes are applied
 * for external interrupt 2  just DIO_FALLING_EDGE, DIO_RISING_EDGE is applied
 *
 * for interrupt 0,1 in active mode applied level must be held to at least 1 clock cycle to
 * be detect by input logic for example to detect rising edge
 * low level must held for at least 1 clock cycle then high level held for at least 1 clock cycle
 *
 * for interrupt 2 it's asynch interrupt so it don't need I/O clock to be on
 * thus it could be used to wake up from sleep modes that I/O clock is off in
 * but any level applied to interrupt 2 pin must be held to at least 50ns to be detected
 *
 * */

/* for this mode is asynch mode, which mean it won't need I/O clock to be on
 * so it could be used to wake up from sleep modes which I/O clock is disabled on
 * in this sleep modes low level need to be held for 2us at least to wake up
 * and to generate an interrupt it will need to held till startup time end
 * startup time is determined by CKSEL,SUT fuses
 * if low level is held to time bigger than 2us but less than startup time
 * then device will wake up but no interrupt will generated
 *
 * in active mode to generate interrupt low level must held till end of currently
 * executed instruction
 * most AVR instruction just need 1 clock cycle to complete but largest instruction
 * will need 3 clock cycles so to make sure low level will generate interrupt
 * it need to held at least 3 clock cycles
 *
 * in this mode interrupt flag always be cleared that as long as low level is held
 * and interrupt is enable the interrupt will fire
 * if interrupt is disabled and this mode is selected and low level was held to enough time
 * to generate interrupt then become high when interrupt is enabled again it won't generate interrupt
 *
 *
 * don't enable nested interrupt inside ISR except when be sure
 * that interrupt low level is not applied any more
 * in this case as long as low level is applied and interrupt is enable
 * locally and globally then interrupt will fire (that could cause memory crash )
 *
 * */
#define DIO_LOW_LEVEL			0

/* this mode will set interrupt flag when either rising edge or falling edge is happen
 *
 * */
#define DIO_TOGGLE_LEVEL		1

/* this mode will set interrupt flag when falling edge happen */
#define DIO_FALLING_EDGE		2

/* this mode will set interrupt flag when rising edge happen */
#define DIO_RISING_EDGE			3
typedef uint8 Dio_InterruptMode ;


/*****************************************************************************/

/* offset of interrupt enable bits */
#define DIO_INTERRUPT_ENABLE_BITS_OFFSET		5

/* this function will enable interrupt locally specified by InterruptID( value from Dio_InterruptType )
 * for interrupt modes other than DIO_LOW_LEVEL if interrupt flag is set when enabled
 * that will fire interrupt immediately after using this function if interrupts is globally enable
 *
 * */

#define Dio_InterruptEnable( InterruptID )			SET_BIT( *GICR_REG,  InterruptID + DIO_INTERRUPT_ENABLE_BITS_OFFSET  )

/* this function disable interrupt locally specified by InterruptID( value from Dio_InterruptType ) */

#define Dio_InterruptDisable( InterruptID )			CLR_BIT( *GICR_REG, InterruptID + DIO_INTERRUPT_ENABLE_BITS_OFFSET  )

/* this function used to read interrupt flag of interrupt specified by InterruptID( value from Dio_InterruptType )
 * if flag is set then interrupt will be fired when enabled locally and globally
 *
 * */

#define Dio_Read_InterruptFlag( InterruptID )		GET_BIT ( *GIFR_REG, InterruptID + DIO_INTERRUPT_ENABLE_BITS_OFFSET )


/* this function used to clear interrupt flag of interrupt specified by InterruptID( value from Dio_InterruptType )
 * if flag is set then interrupt will be fired when enabled locally and globally
 * flag is cleared automatically after executing and return form interrupt ISR
 *
 * don't enable nested interrupt inside ISR before clearing corresponding interrupt flag or
 * memory crash will happen
 *
 * */

#define Dio_Clear_InterruptFlag( InterruptID )		*GIFR_REG = 1 << ( InterruptID + DIO_INTERRUPT_ENABLE_BITS_OFFSET )

/* used to globally enable internal pull up resistors for pin configured as input */

#define Dio_PullUp_GlobalEnable()					SFIOR_REG_BIT->Bit_2 = FALSE ;

/* used to globally disable internal pull up resistors for pin configured as input */
#define Dio_PullUp_GlobalDisable()					SFIOR_REG_BIT->Bit_2 = TRUE ;

/*****************************************************************************/

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

void Dio_Init(void) ;

/*****************************************************************************/

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
void Dio_WritePin ( uint8 PinID, Dio_PinLevel State ) ;


/*****************************************************************************/


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

Dio_PinLevel Dio_ReadPin ( uint8 PinID ) ;

/*****************************************************************************/


/*
 * this function will toggle level of pin which number represented by PinID if o/p
 * if input then it will toggle between pulled up input and tri-stated input
 *
 * */

void Dio_FlipPin ( uint8 PinID ) ;

/*****************************************************************************/


/*
 * this function like Dio_WritePin function put for entire port which its number represented by PortID
 * State bits represent state of every pin in the port MSB of state represent highest pin in the port
 * if bit = 0 corresponding pin will = DIO_LOW and if bit = 1 pin will = DIO_HIGH
 *
 * */
void Dio_WritePort ( uint8 PortID, Dio_PortLevel State ) ;


/*****************************************************************************/


/*
 * this function like Dio_ReadPin function put for entire port which its number represented by PortID
 * MSB of return value will represent level on highest pin in the port
 *
 * */
Dio_PortLevel Dio_ReadPort ( uint8 PortID ) ;

/*****************************************************************************/


/*
 * this function like Dio_FlipPin function put for entire port which its number represented by PortID
 *
 * */
void Dio_FlipPort ( uint8 PortID ) ;

/*****************************************************************************/


/*
 * this function like Dio_WritePort but with input Mask
 * to select specific pins to writes their states if pin's bit in Mask is 0
 * then it's state won't be changed according to State variable
 *
 * */

void Dio_WriteGroup( uint8 PortID, Dio_PortLevel State, Dio_PortLevel Mask ) ;

/*****************************************************************************/


/*
 * this function like Dio_ReadPort but with input Mask
 * to select specific pins to read their states if pin's bit in Mask is 0
 * then it's corresponding bit in return value will be 0 regard less of actual state of pin
 *
 * */
Dio_PortLevel Dio_ReadGroup ( uint8 PortID, Dio_PortLevel Mask ) ;

/*****************************************************************************/


/*
 * this function like Dio_FlipPort but with input Mask
 * to select specific pins to flip their states if pin's bit in Mask is 0
 * then it's state won't be changed
 *
 * */

void Dio_FlipGroup ( uint8 PortID, Dio_PortLevel Mask ) ;


/*****************************************************************************/


/*
 * this function used to set direction of pin which its number represented by PinID
 * to be either input if Direction = DIO_INPUT or to be output if Direction = DIO_OUTPUT
 * */
void Dio_SetPinDirection(  uint8 PinID, Dio_PinDirection Direction ) ;

/*****************************************************************************/


/*
 * this function is like Dio_SetPinDirection but for entire port
 * MSB of Direction represent highest pin direction if 1 then pin is output
 * if 0 then pin is input */

void Dio_SetPortDirection(  uint8 PortID, Dio_PortDirection Direction ) ;


/*****************************************************************************/

/*
 * this function is like Dio_SetPortDirection but wiht mask to select pins to change their direction
 * if pin bit in Mask = 0 then its direction won't be changed according to Direction
 *
 * */

void Dio_SetGroupDirection(  uint8 PortID, Dio_PortDirection Direction,  Dio_PortLevel Mask ) ;


/*****************************************************************************/


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
void Dio_SetInterruptMode ( Dio_InterruptType InterruptID, Dio_InterruptMode  Mode ) ;


/*****************************************************************************/


#endif /* DIO_DIO_H_ */



