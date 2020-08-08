/*
 * 	ExtInterrupt.c
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 *
 */



#include "ExtInterrupt.h"
#include "ExtInterrupt_Private.h"


void (*interrupt_ptr0Void_void) (void) = NULL_PTR ;
void (*interrupt_ptr1Void_void) (void) = NULL_PTR ;
void (*interrupt_ptr2Void_void) (void) = NULL_PTR ;

/*
u8 global_u8GlobalState	 =	GLOBAL_INTERRUPT_STATE ;
u8 global_u8Ex0State	 = INTERRUPT0_STATE ;
u8 global_u8Ex1State	 = INTERRUPT1_STATE ;
u8 global_u8Ex2State	 = INTERRUPT2_STATE ;
*/

/*********************************************************************/



StatusType interrupt_initialization (void)
{

	StatusType RetrunResult = E_OK ;

	/* disable all interrupts till end of initialization */
	CLR_BIT(SREG, SREG_INTERRUPT_BIT) ;

	RetrunResult += interrupt_ex0LocalState(INTERRUPT_DISABLE) ;
	RetrunResult += interrupt_ex1LocalState(INTERRUPT_DISABLE) ;
	RetrunResult += interrupt_ex2LocalState(INTERRUPT_DISABLE) ;

	RetrunResult += interrupt_ex0FireMode (INTERRUPT0_FIRE_MODE) ;
	RetrunResult += interrupt_ex1FireMode (INTERRUPT1_FIRE_MODE) ;
	RetrunResult += interrupt_ex2FireMode (INTERRUPT2_FIRE_MODE) ;

	interrupt_ex0ClearFlag () ;
	interrupt_ex1ClearFlag () ;
	interrupt_ex2ClearFlag () ;

	RetrunResult += interrupt_ex0LocalState(INTERRUPT0_STATE) ;
	RetrunResult += interrupt_ex1LocalState(INTERRUPT1_STATE) ;
	RetrunResult += interrupt_ex2LocalState(INTERRUPT2_STATE) ;

	RetrunResult += interrupt_globalState( GLOBAL_INTERRUPT_STATE ) ;



	return   private_errorChecking (RetrunResult) ;

} /* interrupt_initialization */




/*********************************************************************/




StatusType interrupt_globalState( uint8 copy_u8state )
{

	StatusType ReturnResult = E_OK ;

	if(INTERRUPT_ENABLE == copy_u8state || INTERRUPT_DISABLE == copy_u8state)
	{

		ASSIGN_BIT(SREG, SREG_INTERRUPT_BIT, copy_u8state);


	} /* if  */
	else
	{
		ReturnResult = E_NOT_OK ;
	}

	return   ReturnResult ;

} /* interrupt_globalState */




/*********************************************************************/

StatusType interrupt_ex0LocalState( uint8 copy_u8state)
{

	StatusType ReturnResult = E_OK ;

	if(INTERRUPT_ENABLE == copy_u8state || INTERRUPT_DISABLE == copy_u8state)
	{

		ASSIGN_BIT(GISR, GISR_INTERRUPT0_BIT, copy_u8state);
		return   E_OK ;

	} /* if  */

	else
	{
		ReturnResult = E_NOT_OK ;
	}

	return ReturnResult ;

} /* interrupt_ex0LocalState */





/*********************************************************************/




StatusType interrupt_ex1LocalState( uint8 copy_u8state)
{
	StatusType ReturnResult = E_OK ;

	if(INTERRUPT_ENABLE == copy_u8state || INTERRUPT_DISABLE == copy_u8state)
	{

		ASSIGN_BIT(GISR, GISR_INTERRUPT1_BIT, copy_u8state);

	} /* if  */
	else
	{
		ReturnResult = E_NOT_OK ;
	}

	return ReturnResult ;

} /* interrupt_ex1LocalState */





/*********************************************************************/




StatusType interrupt_ex2LocalState( uint8 copy_u8state)
{
	StatusType ReturnResult = E_OK ;

	if(INTERRUPT_ENABLE == copy_u8state || INTERRUPT_DISABLE == copy_u8state)
	{

		ASSIGN_BIT(GISR, GISR_INTERRUPT2_BIT, copy_u8state);
	/*	global_u8Ex2State =  copy_u8state ; */

	} /* if  */

	else
	{
		ReturnResult = E_NOT_OK ;
	}

	return ReturnResult ;

} /* interrupt_ex2LocalState */




/*********************************************************************/




void interrupt_ex0ClearFlag(void)
{

	ASSIGN_BIT(GIFR, GIFR_INTERRUPT0_BIT, CLEAR_FLAGE);

	return ;

}	/* interrupt_ex0ClearFlag */




/*********************************************************************/




void interrupt_ex1ClearFlag(void)
{
	ASSIGN_BIT(GIFR, GIFR_INTERRUPT1_BIT, CLEAR_FLAGE);

	return ;

}	/* interrupt_ex1ClearFlag */




/*********************************************************************/




void interrupt_ex2ClearFlag(void)
{
	ASSIGN_BIT(GIFR, GIFR_INTERRUPT2_BIT, CLEAR_FLAGE);

	return ;

}	/* interrupt_ex2ClearFlag */




/*********************************************************************/


void interrupt_ex0ReadFlag(uint8 * copy_pu8Data)
{

	*copy_pu8Data = GET_BIT( GIFR, GIFR_INTERRUPT0_BIT ) ;

	return  ;

}	/* interrupt_ex0ReadFlag */



/*********************************************************************/



void interrupt_ex1ReadFlag(uint8 * copy_pu8Data)
{
	*copy_pu8Data = GET_BIT(GIFR,GIFR_INTERRUPT1_BIT );

	return ;

}	/* interrupt_ex1ReadFlag */




/*********************************************************************/



void interrupt_ex2ReadFlag(uint8 * copy_pu8Data)
{
	*copy_pu8Data = GET_BIT(GIFR,GIFR_INTERRUPT2_BIT );

	return ;

}	/* interrupt_ex2ReadFlag */



StatusType interrupt_ex0FireMode (uint8 copy_u8FireMode)
{
	StatusType RetrunResult = E_OK ;


	if (INTERRUPT0_LOW_LEVEL==copy_u8FireMode || INTERRUPT0_ANY_CHANGE==copy_u8FireMode || INTERRUPT0_FALLING_EDGE==copy_u8FireMode || INTERRUPT0_RISING_EDGE==copy_u8FireMode)
	{
		RetrunResult += interrupt_ex0LocalState(INTERRUPT_DISABLE);

		ASSIGN_BIT(MCUCR, MCUCR_INTERRUPT0_BIT0, ( copy_u8FireMode & (1 << (0) ) ) >> (0) );
		ASSIGN_BIT(MCUCR, MCUCR_INTERRUPT0_BIT1, ( copy_u8FireMode & (1 << (1) ) ) >> (1) );

		interrupt_ex0ClearFlag();
		interrupt_ex0LocalState(INTERRUPT_ENABLE);

	}	/* if  */
	else
	{
		RetrunResult = E_NOT_OK ;
	}

	return   E_NOT_OK ;

}	/* interrupt_ex0FireMode */




/*********************************************************************/


StatusType interrupt_ex1FireMode (uint8 copy_u8FireMode)
{
	StatusType RetrunResult = E_OK ;

	if (INTERRUPT1_LOW_LEVEL==copy_u8FireMode || INTERRUPT1_ANY_CHANGE==copy_u8FireMode || INTERRUPT1_FALLING_EDGE==copy_u8FireMode || INTERRUPT1_RISING_EDGE==copy_u8FireMode)
	{
		RetrunResult += interrupt_ex1LocalState(INTERRUPT_DISABLE);

		ASSIGN_BIT(MCUCR, MCUCR_INTERRUPT1_BIT0, ( copy_u8FireMode & (1 << (0) ) ) >> (0) );
		ASSIGN_BIT(MCUCR, MCUCR_INTERRUPT1_BIT1, ( copy_u8FireMode & (1 << (1) ) ) >> (1) );

		interrupt_ex1ClearFlag();
		interrupt_ex1LocalState(INTERRUPT_ENABLE);

	} /* if  */
	else
	{
		RetrunResult = E_NOT_OK ;
	}

	return  private_errorChecking (RetrunResult) ;


}	/* interrupt_ex1FireMode */



/*********************************************************************/


StatusType interrupt_ex2FireMode (uint8 copy_u8FireMode)
{

	StatusType RetrunResult = E_OK ;

	if (INTERRUPT2_FALLING_EDGE==copy_u8FireMode || INTERRUPT2_RISING_EDGE==copy_u8FireMode)
	{

		RetrunResult += interrupt_ex2LocalState(INTERRUPT_DISABLE);

		ASSIGN_BIT(MCUCSR, MCUCSR_INTERRUPT2_BIT, copy_u8FireMode );

		interrupt_ex2ClearFlag();
		interrupt_ex2LocalState(INTERRUPT_ENABLE);

	}	/* if  */

	else
	{
		RetrunResult = E_NOT_OK ;
	}

	return  private_errorChecking (RetrunResult) ;


}	/* interrupt_ex2FireMode */





/*********************************************************************/



void interrupt_external0 (void (* copy_ptr0Void_void )(void))
{

	interrupt_ptr0Void_void  = copy_ptr0Void_void ;

	return ;

}	/* interrupt_external0 */



/*********************************************************************/

void interrupt_external1 (void (* copy_ptr1Void_void) (void))
{

	interrupt_ptr1Void_void  = copy_ptr1Void_void ;

	return ;


}	/* interrupt_external0 */



/*********************************************************************/





void interrupt_external2 ( void (* copy_ptr2Void_void) (void) )
{
	/*if ( INTERRUPT_DISABLE==global_u8GlobalState || INTERRUPT_DISABLE==global_u8Ex2State )
	{
		return E_NOT_OK ;
	}*/
	interrupt_ptr2Void_void  = copy_ptr2Void_void ;

	return ;

}	/* interrupt_external2 */


/*********************************************************************/



private_exInterrupt(VECTOR_INT0)
{
	interrupt_ptr0Void_void () ;

}	/* private_exInterrupt */

/*********************************************************************/


private_exInterrupt(VECTOR_INT1)
{
	interrupt_ptr1Void_void () ;

}	/* private_exInterrupt */


/*********************************************************************/


private_exInterrupt(VECTOR_INT2)
{
	interrupt_ptr2Void_void () ;

}	/* private_exInterrupt */





StatusType private_errorChecking (StatusType copy_validation)
{
	StatusType ReturnResult = E_OK ;
	if(! copy_validation)
	{
	}
	else
	{
		ReturnResult = E_NOT_OK ;
	}

	return E_NOT_OK ;

}	/* private_errorChecking */
