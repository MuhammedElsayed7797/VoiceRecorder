/*
 * EEPROM_24C512.c
 *
 * Created on: Mar 18, 2020
 * Author: Bebo
 */

#define  EEPROM_24C512_SOURCE_GAURD

#include "EEPROM_24C512.h"

#define EE24C512_BASIC_ADDRESS	0b01010000


void EE24C512_ISR (void) ;

void (* TWI_NotificationFunction_Temp) (void) ;

volatile boolean EE24C512_BusyFlag = FALSE ;

boolean EE24C512_ReadAddressFlag = FALSE ;

boolean EE24C512_NotificationFlag = FALSE ;


uint8 * ISR_ReadDataAdress = NULL_PTR ;
uint8  ISR_ReadDataSize = 0 ;


uint8 * ISR_WriteDataAdress = NULL_PTR ;
uint8  ISR_WriteDataSize = 0 ;
uint8 EEPROM_Address = 0 ;

EE24C512_NOTIFICATION_DATA_TYPE * EE24C512_NotificationAddress ;
EE24C512_NOTIFICATION_DATA_TYPE  EE24C512_NotificationValue ;
EE24C512_NOTIFICATION_DATA_TYPE  EE24C512_NotificationMask ;

/********************************************************************************************************/

/* used to write in EEPROM memory
 * on the same bus max number of 24C512 memory is 8 so EEPROM_Number has number of memory to
 * talk with according to its A2,A1,A0 pins  from 000 ~ 111 if EEPROM_Number = 6
 * then it will send to memory which its A0 is low and A1,A2 is hihg
 *
 * Byte_Address refer to first location byte inside the memory to write in
 *
 * user must has array of bytes to be transmitted to EEPROM this array can't be modified
 * before EE24C512_Write is complete  except for  user certain purpose
 *
 * Note : this array must have first 2 bytes on it be empty
 *
 * TransmittedData is the address of first byte in this array
 *
 * DataSize determine ( size of this array - 2 ), max is 128
 *
 * this function is could be used to initiate internal address of EEPROM to Sequential Read operation
 * start form Byte_Address
 *
 * */

StatusType EE24C512_Write ( uint8 EEPROM_Number, uint16* Byte_Address, uint8* TransmittedData, uint8 DataSize )
{
	StatusType ReturnResult = E_OK ;

	if( FALSE == EE24C512_BusyFlag )
	{
		/* set busy flag */
		EE24C512_BusyFlag = TRUE ;

		/* change notification function called inside TWI ISR and save old one in temp variable */
		TWI_NotificationFunction_Temp = TWI_NotificationFunction ;
		TWI_NotificationFunction = EE24C512_ISR ;

		/* evaluate TWI_MT parameters */
		EEPROM_Address = EE24C512_BASIC_ADDRESS | EEPROM_Number ;
		ISR_WriteDataSize = DataSize + 2 ;
		ISR_WriteDataAdress = (uint8 *)Byte_Address ;


		if( 0 == DataSize )
		{
			/* in this case just send two bytes */
			EEPROM_Number = ISR_WriteDataAdress [0] ;
			ISR_WriteDataAdress [0] = ISR_WriteDataAdress [1] ;
			ISR_WriteDataAdress [1] = EEPROM_Number ;
		}
		else
		{
			/* in this case first two bytes to send will be EEPROM byte address */
			TransmittedData[0] = ISR_WriteDataAdress[1] ;
			TransmittedData[1] = ISR_WriteDataAdress[0] ;

			ISR_WriteDataAdress = TransmittedData ;
		}

		/* set device in Master transmitter mode */
		TWI_MT(ISR_WriteDataAdress, EEPROM_Address, ISR_WriteDataSize );

	}
	else
	{
		ReturnResult = E_NOT_OK ;
	}

	return ReturnResult ;
}


/********************************************************************************************************/


/* used to read EEPROM memory
 * on the same bus max number of 24C512 memory is 8 so EEPROM_Number has number of memory to
 * talk with according to its A2,A1,A0 pins  from 000 ~ 111 if EEPROM_Number = 6
 * then it will send to memory which its A0 is low and A1,A2 is high
 *
 * Byte_Address refer to first location byte inside the memory to write in
 *
 * user must has array of bytes to be receive data from EEPROM this array can't be modified
 * before EE24C512_Write is complete  except for  user certain purpose
 * ReceivedData is the address of first byte in this array
 * DataSize determine size of this array
 *
 * this function is could be used to read from internal address of EEPROM without need to send
 * a new address if Byte_Address is equal to NULL_PTR
 *
 * */


StatusType EE24C512_Read ( uint8 EEPROM_Number, uint16* Byte_Address, uint8* ReceivedData, uint8 DataSize )
{
	StatusType ReturnResult = E_OK ;

	if( FALSE == EE24C512_BusyFlag  )
	{

		/* set busy flag */
		EE24C512_BusyFlag = TRUE ;

		/* change notification function called inside TWI ISR and save old one in temp variable */
		TWI_NotificationFunction_Temp = TWI_NotificationFunction ;
		TWI_NotificationFunction = EE24C512_ISR ;

		/* evaluate TWI_MR parameters */
		EEPROM_Address = EE24C512_BASIC_ADDRESS | EEPROM_Number ;
		ISR_ReadDataSize = DataSize  ;
		ISR_ReadDataAdress = ReceivedData ;

		if( NULL_PTR != Byte_Address )
		{
			EE24C512_ReadAddressFlag = TRUE ;

			/* evaluate TWI_MT parameters to send Read address*/
			ISR_WriteDataSize = 2  ;
			ISR_WriteDataAdress = (uint8 *) Byte_Address ;

			EEPROM_Number = ISR_WriteDataAdress [0] ;
			ISR_WriteDataAdress [0] = ISR_WriteDataAdress [1] ;
			ISR_WriteDataAdress [1] = EEPROM_Number ;

			/* in this case just send two bytes */
			TWI_MT( ISR_WriteDataAdress, EEPROM_Address, ISR_WriteDataSize );

		}
		else
		{
			/* set device in Master transmitter mode */
			TWI_MR( ISR_ReadDataAdress, EEPROM_Address, ISR_ReadDataSize );
		}

	}
	else
	{
		ReturnResult = E_NOT_OK;
	}
	return ReturnResult ;
}


/********************************************************************************************************/


/* this function used to assign notification when a certain EEPROM process is finished
 * if any EEPROM function call happened including EE24C512_Notification
 * before this notification then it will return E_NOT_OK
 * notification will assign DestinationValue into location of DestinationAddress according to ValueMask
 * bits corresponding to cleared bits in ValueMask won't change
 * this could be used to generate a software interrupt when Read/Write EEPROM function is called
 *
 * this function allowed to be called just before calling EEPROM READ/WRITE functions
 * if called after calling them and before finishing their operation it will retrun E_NOT_OK
 *
 * */

StatusType EE24C512_Notification ( EE24C512_NOTIFICATION_DATA_TYPE * DestinationAddress, EE24C512_NOTIFICATION_DATA_TYPE DestinationValue, EE24C512_NOTIFICATION_DATA_TYPE ValueMask )
{
	StatusType ReturnResult = E_OK ;

	if( FALSE == EE24C512_BusyFlag )
	{
		EE24C512_NotificationFlag = TRUE ;
		EE24C512_NotificationAddress = DestinationAddress ;
		EE24C512_NotificationValue = DestinationValue ;
		EE24C512_NotificationMask = ValueMask ;


	}
	else
	{
		ReturnResult = E_NOT_OK;
	}
	return ReturnResult ;
}





/********************************************************************************************************/


void EE24C512_ISR (void)
{
	EE24C512_NOTIFICATION_DATA_TYPE TempVariable = 0 ;
	switch (TWI_CurrentState)
	{
		case (TWI_TRANSMIT_SLA_W_NACK) :
		{
			__asm("NOP");
			TWI_MT(ISR_WriteDataAdress, EEPROM_Address, ISR_WriteDataSize );

			break ;
		}

		case (TWI_TRANSMIT_DATA_ACK) :
		{
			if ( TWI_Internal_DataIndex == ISR_WriteDataSize )
			{
					
				/* case send Read address is finished and want to read from EEPROMS
				 * this happen when using EE24C512_Read and Byte_Address = NULL_PTR */
				if ( TRUE == EE24C512_ReadAddressFlag )
				{
					TWI_MR( ISR_ReadDataAdress, EEPROM_Address, ISR_ReadDataSize );
					EE24C512_ReadAddressFlag = FALSE ;
				}
				else
				{
					/* clear notification flag */
					EE24C512_BusyFlag = FALSE ;

					if ( TRUE == EE24C512_NotificationFlag)
					{
						/* case there's a notification event clear notification flag and execute event */
						EE24C512_NotificationFlag = FALSE ; 

						TempVariable = *EE24C512_NotificationAddress ;
						TempVariable |= (EE24C512_NotificationValue & EE24C512_NotificationMask ) ;
						TempVariable &= ~(EE24C512_NotificationValue & EE24C512_NotificationMask ) ;
						
						/* retrun old TWI notification function */
						*EE24C512_NotificationAddress = TempVariable ;
					}
					else
					{
						
					}
				}
			}
			else
			{
			}

			break ;
		}

		case (TWI_TRANSMIT_SLA_R_NACK) :
		{
			TWI_MR( ISR_ReadDataAdress, EEPROM_Address, ISR_ReadDataSize );

			break ;
		}

		case (TWI_M_RECEIVE_DATA_NACK) :
		{
			EE24C512_BusyFlag = FALSE ;

			if ( TRUE == EE24C512_NotificationFlag)
			{
				/* case there's a notification event clear notification flag and execute event */
				EE24C512_NotificationFlag = FALSE ; 

				TempVariable = *EE24C512_NotificationAddress ;
				TempVariable |= (EE24C512_NotificationValue & EE24C512_NotificationMask ) ;
				TempVariable &= ~(EE24C512_NotificationValue & EE24C512_NotificationMask ) ;
						
				/* retrun old TWI notification function */
				*EE24C512_NotificationAddress = TempVariable ;
			}
			else
			{
					
			}
			break ;
		}
	}

	return ;

}




