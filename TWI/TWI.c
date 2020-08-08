/*
 * 	TWI.c
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 *
 */

#define  TWI_SOURCE_GAURD

#include "../TWI/TWI.h"

#include "../TWI/TWI_Private.h"

uint8 TWI_Test = 0x00u ;

/* used to save address and R/W bit which will be send after START */
uint8 TWI_Internal_SLA_R_W	= 0x00u ;

/* used to index data transmitted or received */
volatile uint8 TWI_Internal_DataIndex = 0x00u ;

/* used to hold data start address in every TWI mode */
uint8 * TWI_InternalMT_DataAddress	= 0x00u ;
uint8 * TWI_InternalMR_DataAddress	= 0x00u ;
uint8 * TWI_InternalST_DataAddress	= 0x00u ;
uint8 * TWI_InternalSR_DataAddress	= 0x00u ;


boolean TWI_SlaveFlag = FALSE ;

/*determine number of data packets transmitted per START condition when device in master transmitter  mode */
uint8 TWI_MT_DataSize  ;

/*determine number of data packets transmitted per START condition when device in master receiver mode */
uint8 TWI_MR_DataSize  ;

/*determine number of data packets transmitted per START condition when device in m slave transmitter mode */
uint8 TWI_ST_DataSize  ;

/*determine number of data packets transmitted per START condition when device in slave receiver mode */
uint8 TWI_SR_DataSize  ;

/* used to indicate state of TWI saved in TWSR register, just used inside Notification function */
volatile uint8 TWI_CurrentState = 0x00u ;

/* used to indicate modes that called by user and its operation not finished yet */
struct
{
	uint8 MT_Mode : 1 ;
	uint8 MR_Mode : 1 ;
	uint8 ST_Mode : 1 ;
	uint8 SR_Mode : 1 ;

}TWI_ModeBusy;

/* those will be assigned to function which will be called before and after and during data transmission, and in case of error */
void (* TWI_NotificationFunction ) (void) =  NULL_PTR ;



/* when called enable TWI hardware & interrupt and initiate its settings
 * used to initialize SCL frequency  of TWI according to equation of
 * CPU_CLOCK/(16 + ( 2 * TWI_BIT_RATE * 2^(TWI_PRESCALER) ) )
 * max SCL frequency is 400KHz
 * also used to initiate 7 bit address of device according to lowest 7 bits in TWI_DEVICE_ADDRESS
 */
void TWI_Init (void)
{

	/* initialize bit rate */
	(TWAR_REG->RegisterByte) = ( TWI_DEVICE_ADDRESS << 1 ) ;

	(TWBR_REG->RegisterByte) = TWI_BIT_RATE ;

	/* initialize prescaler */
	(TWSR_REG->RegisterBits).Bit_0 = TWI_PRESCALER & 0b01 ;
	(TWSR_REG->RegisterBits).Bit_1 = (TWI_PRESCALER >> 1 )  ;

	/* initialize control register to enable TWI hardwar and interupt  */
	(TWCR_REG->RegisterByte) = 0b10000101 ;

	TWI_ModeBusy.MR_Mode  = 0 ;
	TWI_ModeBusy.MT_Mode  = 0 ;
	TWI_ModeBusy.SR_Mode  = 0 ;
	TWI_ModeBusy.ST_Mode  = 0 ;



	/* enable port c pin 1,0 pull up resistors, value -> 20~40 kohm */
	(DDRC_REG->RegisterBits).Bit_0 =  FALSE ;
	(DDRC_REG->RegisterBits).Bit_1 =  FALSE ;
	(PORTC_REG->RegisterBits).Bit_0 =  TRUE ;
	(PORTC_REG->RegisterBits).Bit_1 =  TRUE ;

	return ;
}


/***********************************************************************************/

/*
 * disable TWI hardware and interrupt
 * */
void TWI_DeInit (void)
{
	(TWCR_REG->RegisterByte) = 0b10000000 ;

	TWI_ModeBusy.MR_Mode  = 0 ;
	TWI_ModeBusy.MT_Mode  = 0 ;
	TWI_ModeBusy.SR_Mode  = 0 ;
	TWI_ModeBusy.ST_Mode  = 0 ;

	return ;
}



/***********************************************************************************/

/*
 * this function called when device want to become a master and transmit data
 * user must have an array of data to be transmitted
 * TransmittedData hold address of first byte to be transmitted
 * SlaveAddress hold the address of slave device to receive data
 * DataSize hold number of bytes to be transmitted
 *
 * Note: in case of this function is called before MT or MR previous transmission requested and waiting
 * or didn't finished yet then it will return E_NOT_OK
 *
 * array of data to be transmitted mustn't be modified until transmission is finished
 * except user was intend to modify it for some reason
 *
 * its recommended to call this function in Notification function under certain user condition,
 * indicating that current transmission is ended
 * to avoid calling it while another transmission operation processed
 * and to avoid pulling over E_OK status type
 *
 * Important: to avoid polling on E_OK inside notification function because it executed inside isr
 *
 * */

StatusType TWI_MT ( uint8 * TransmittedData, uint8  SlaveAddress, uint8 DataSize )
{
	StatusType ReturnResult = E_OK ;

	if ( FALSE == ( TWI_ModeBusy.MT_Mode | TWI_ModeBusy.MR_Mode ) )
	{

		TWI_ModeBusy.MT_Mode = TRUE ;

		TWI_MT_DataSize = DataSize ;

		/* assign address of slave + Write bit
		 * and assign first data byte address */
		TWI_Internal_SLA_R_W = (SlaveAddress << 1)  ;
		TWI_InternalMT_DataAddress = TransmittedData ;

		/* in case of called of notification function to make repeated start*/
		(TWCR_REG->RegisterBits).Bit_4 = FALSE ;

		/* to initiate start */
		(TWCR_REG->RegisterBits).Bit_5 = TRUE ;

	}
	else
	{
		ReturnResult =  E_NOT_OK ;
	}


	return ReturnResult ;
}


/***********************************************************************************/


/*
 * this function called when device want to become a master and receive data
 * user must have an array of data to data received in
 * TransmittedData hold address of first byte to be transmitted
 * SlaveAddress hold the address of slave device to transmitt data
 * DataSize hold number of bytes to be transmitted
 *
 * Note: in case of this function is called before MT or MR previous transmission requested and waiting
 * or didn't finished yet then it will return E_NOT_OK
 *
 * array of data to be received mustn't be modified until transmission is finished
 * except user was intend to modify it for some reason
 *
 * its recommended to call this function in Notification function under certain user condition,
 * indicating that current transmission is ended
 * to avoid calling it while another transmission operation processed
 * and to avoid pulling over E_OK status type
 *
 * */

StatusType TWI_MR ( uint8 * ReceivedData, uint8  SlaveAddress, uint8 DataSize )
{
	StatusType ReturnResult = E_OK ;

	if ( FALSE == ( TWI_ModeBusy.MT_Mode | TWI_ModeBusy.MR_Mode ) )
	{
		TWI_ModeBusy.MR_Mode = TRUE ;

		TWI_MR_DataSize = DataSize ;

		/* assign address of slave + read bit */
		TWI_Internal_SLA_R_W = ( (SlaveAddress << 1)  | 0x01u ) ;
		TWI_InternalMR_DataAddress = ReceivedData ;

		/* in case of called of notification function to make repeated start */
		(TWCR_REG->RegisterBits).Bit_4 = FALSE ;

		(TWCR_REG->RegisterBits).Bit_5 = TRUE ;

	}
	else
	{
		ReturnResult =  E_NOT_OK ;
	}

	return ReturnResult ;
}


/***********************************************************************************/

/*
 * this function called when device want to become a slave and transmit data when device
 * receive its address+R on the bus
 *
 * user must have an array of data to be transmitted
 * TransmittedData hold address of first byte to be transmitted
 * DataSize hold number of bytes to be transmitted
 *
 * Note: in case of this function is called before ST previous transmission requested and waiting
 * or didn't finished yet then it will return E_NOT_OK
 *
 * array of data to be transmitted mustn't be modified until transmission is finished
 * except user was intend to modify it for some reason
 *
 * its recommended to call this function in Notification function under certain user condition,
 * indicating that current transmission is ended
 * to avoid calling it while another ST transmission operation processed
 * and to avoid pulling over E_OK status type
 *
 * this function won't start data transmission it just will make device
 * respond with ACK when its address is received on the bus
 *
 * Note :  when using this function the device will respond with ACK when its address on bus
 * whatever address+ R or + W  so it's recommend to initialize both ST and SR together to avoid sending
 * or receiving non useful data, and when one of them finished recommend to reinitialized till both are complete
 *
 * Important: to avoid polling on E_OK inside notification function because it executed inside isr
 *
 * */

StatusType TWI_ST ( uint8 * TransmittedData, uint8 DataSize )
{
	StatusType ReturnResult = E_OK ;


	if ( FALSE == ( TWI_ModeBusy.ST_Mode  ) )
	{
		/* set busy and slave flag */
		TWI_ModeBusy.ST_Mode = TRUE ;
		TWI_SlaveFlag = TRUE ;

		/* determine data size and address of array to receive data */
		TWI_ST_DataSize = DataSize ;
		TWI_InternalST_DataAddress = TransmittedData ;

		/* set ACK bit */
		(TWCR_REG->RegisterBits).Bit_6 = TRUE ;

	}
	else
	{
		ReturnResult =  E_NOT_OK ;
	}

	return ReturnResult ;
}


/***********************************************************************************/


/*
 * this function called when device want to become a slave and receive data when device
 * receive its address+R on the bus
 *
 * user must have an array of data to be received
 * ReceivedData hold address of first byte to be received
 * DataSize hold number of bytes to be received
 *
 * Note: in case of this function is called before SR previous transmission requested and waiting
 * or didn't finished yet then it will return E_NOT_OK
 *
 * array of data to be transmitted mustn't be modified until transmission is finished
 * except user was intend to modify it for some reason
 *
 * its recommended to call this function in Notification function under certain user condition,
 * indicating that current transmission is ended
 * to avoid calling it while another ST transmission operation processed
 * and to avoid pulling over E_OK status type
 *
 * this function won't start data transmission it just will make device
 * respond with ACK when its address is received on the bus
 *
 * Note :  when using this function the device will respond with ACK when its address on bus
 * whatever address+ R or + W  so it's recommend to initialize both ST and SR together to avoid sending
 * or receiving non useful data, and when one of them finished recommend to reinitialized till both are complete
 *
 * Important: to avoid polling on E_OK inside notification function because it executed inside isr
 *
 * */

StatusType TWI_SR ( uint8 * ReceivedData, uint8 DataSize )
{
	StatusType ReturnResult = E_OK ;


	if ( FALSE == ( TWI_ModeBusy.ST_Mode  ) )
	{

		/* set busy and slave flag */
		TWI_ModeBusy.SR_Mode = TRUE ;
		TWI_SlaveFlag = TRUE ;

		/* determine data size and address of array to receive data */
		TWI_SR_DataSize = DataSize ;
		TWI_InternalSR_DataAddress = ReceivedData ;

		/* set ACK bit */
		(TWCR_REG->RegisterBits).Bit_6 = TRUE ;

	}
	else
	{
		ReturnResult =  E_NOT_OK ;
	}

	return ReturnResult ;
}


/***********************************************************************************/

/*
 * this API used to disable or enable general call recognition
 * according to value of Enable_GC, if TRUE -> enable
 * */

void TWI_GeneralCall ( boolean Enable_GC )
{

	/* write 1 to enable general call recognition and 0 to disable */
	(TWAR_REG->RegisterBits).Bit_0 = Enable_GC ;

	return ;
}


/***********************************************************************************/

/* used to generate STOP on the bus in case of master
 * and to be non addressed slave in case of slave
 * */
void TWI_TransmissionSTOP ( void )
{

	(TWCR_REG->RegisterBits).Bit_4 = TRUE ;
}

/***********************************************************************************/

/* used to virtually disconnect device from  TWI bus
 * device won't respond with ACK when its address received on  TWI bus */

void TWI_UnSlave (void)
{
	(TWCR_REG->RegisterBits).Bit_6 = TRUE ;
	return ;
}

/***********************************************************************************/



ISR ( TWI_ISR )
{

	TWI_CurrentState = (TWSR_REG->RegisterByte) & 0xF8 ;

	switch ( TWI_CurrentState )
	{

		case (TWI_ERROR_STATE) :
		{

			(TWCR_REG->RegisterByte) = 0b10010101 ;

			#if TWI_ERROR_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_ERROR_NOTIFICATION */

			break ;
		}
		case (TWI_TRANSMIT_START) :
		{
			/* initialize data index */
			TWI_Internal_DataIndex = 0 ;

			/* write SLA+R/W to TWDR clear TWSTA  */
			(TWDR_REG->RegisterByte) =  TWI_Internal_SLA_R_W;

			(TWCR_REG->RegisterBits).Bit_5 = FALSE ;

			#if TWI_TRANSMIT_START_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_TRANSMIT_START_NOTIFICATION */

			break ;
		}

		case (TWI_TRANSMIT_REPEATED) :
		{
			/* initialize data index */
			TWI_Internal_DataIndex = 0 ;

			/* write SLA+R/W to TWDR clear TWSTA  */
			(TWDR_REG->RegisterByte) =  TWI_Internal_SLA_R_W;

			(TWCR_REG->RegisterBits).Bit_5 = FALSE ;

			#if TWI_TRANSMIT_REPEATED_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_TRANSMIT_REPEATED_NOTIFICATION */

			break ;

		}

		case (TWI_TRANSMIT_SLA_W_ACK) :
		{

			TWI_Test = TWI_InternalMT_DataAddress[TWI_Internal_DataIndex] ;

			(TWDR_REG->RegisterByte) =  TWI_InternalMT_DataAddress[TWI_Internal_DataIndex] ;

			#if TWI_TRANSMIT_SLA_W_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_TRANSMIT_SLA_W_ACK_NOTIFICATION */

			break ;
		}
		case (TWI_TRANSMIT_SLA_W_NACK) :
		{

			/*  generate STOP */
			(TWCR_REG->RegisterBits).Bit_4 = TRUE ;
			TWI_ModeBusy.MT_Mode = FALSE ;


			#if TWI_TRANSMIT_SLA_W_NACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_TRANSMIT_SLA_W_NACK_NOTIFICATION */

			break ;

		}

		case (TWI_TRANSMIT_DATA_ACK) :
		{
			TWI_Internal_DataIndex ++ ;

			if ( TWI_Internal_DataIndex < TWI_MT_DataSize)
			{
				TWI_Test = TWI_InternalMT_DataAddress[TWI_Internal_DataIndex] ;
				(TWDR_REG->RegisterByte) =  TWI_InternalMT_DataAddress[TWI_Internal_DataIndex] ;
			}
			else
			{
				/* generate STOP  and clear busy flag */
				(TWCR_REG->RegisterBits).Bit_4 = TRUE ;
				TWI_ModeBusy.MT_Mode = FALSE ;

			}

			#if TWI_TRANSMIT_DATA_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_TRANSMIT_DATA_ACK_NOTIFICATION */

			break ;
		}

		case (TWI_TRANSMIT_DATA_NACK) :
		{

			/* generate STOP  and clear busy flag */
			(TWCR_REG->RegisterBits).Bit_4 = TRUE ;
			TWI_ModeBusy.MT_Mode = FALSE ;


			#if TWI_TRANSMIT_DATA_NACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_TRANSMIT_DATA_NACK_NOTIFICATION */

			break ;
		}

		case (TWI_ARBITRATION_LOST) :
		{

			TWI_ModeBusy.MT_Mode = FALSE ;


			#if TWI_ARBITRATION_LOST_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_ARBITRATION_LOST_NOTIFICATION */

			break ;
		}

		case (TWI_TRANSMIT_SLA_R_ACK) :
		{

			/* check if master will read one byte */
			if ( TWI_MR_DataSize > 0x01u )
			{
				/* send ACK */
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;
				TWI_MR_DataSize -- ;
			}
			else
			{
				/* send NACK*/
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;

			}

			#if TWI_TRANSMIT_SLA_R_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_TRANSMIT_SLA_R_ACK_NOTIFICATION */

			break ;
		}

		case (TWI_TRANSMIT_SLA_R_NACK) :
		{

			/*  generate STOP and clear busy flag */
			(TWCR_REG->RegisterBits).Bit_4 = TRUE ;
			
			TWI_ModeBusy.MT_Mode = FALSE ;
			TWI_ModeBusy.MR_Mode = FALSE ;


			#if TWI_TRANSMIT_SLA_R_NACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_TRANSMIT_SLA_R_NACK_NOTIFICATION */

			break ;
		}

		case (TWI_M_RECEIVE_DATA_ACK) :
		{

			TWI_InternalMR_DataAddress[TWI_Internal_DataIndex] = (TWDR_REG->RegisterByte) ;
			TWI_Internal_DataIndex ++ ;

			if (TWI_Internal_DataIndex < TWI_MR_DataSize )
			{
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;
			}
			else
			{
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;
			}

			#if TWI_M_RECEIVE_DATA_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_M_RECEIVE_DATA_ACK_NOTIFICATION */

			break ;
		}

		case (TWI_M_RECEIVE_DATA_NACK) :
		{

			TWI_InternalMR_DataAddress[TWI_Internal_DataIndex] = (TWDR_REG->RegisterByte) ;

			(TWCR_REG->RegisterBits).Bit_6 = TWI_SlaveFlag ;
			(TWCR_REG->RegisterBits).Bit_4 = TRUE ;

			TWI_ModeBusy.MR_Mode = FALSE ;


			#if TWI_M_RECEIVE_DATA_NACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_M_RECEIVE_DATA_NACK_NOTIFICATION */

			break ;
		}

		case (TWI_RECEIVE_SLA_W_ACK) :
		{

			/* initialize data index */
			TWI_Internal_DataIndex = 0 ;

			/* check if slave will read one byte */
			if ( TWI_SR_DataSize > 0x01u )
			{
				/* send ACK */
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;
			}
			else
			{
				/* send NACK*/
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;

			}

			#if TWI_RECEIVE_SLA_W_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_RECEIVE_SLA_W_ACK_NOTIFICATION */

			break ;
		}

		case (TWI_ARBITRATION_LOST_SLA_W) :
		{
			TWI_ModeBusy.MT_Mode = FALSE ;
			TWI_ModeBusy.MR_Mode = FALSE ;

			/* initialize data index */
			TWI_Internal_DataIndex = 0 ;

			/* check if slave will read one byte */
			if ( TWI_SR_DataSize > 0x01u )
			{
				/* send ACK */
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;
			}
			else
			{
				/* send NACK*/
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;
			}

			#if TWI_ARBITRATION_LOST_SLA_W_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_ARBITRATION_LOST_SLA_W_NOTIFICATION */

			break ;
		}

		case (TWI_S_RECEIVE_GC_ACK) :
		{

			/* initialize data index */
			TWI_Internal_DataIndex = 0 ;

			/* check if slave will read one byte */
			if ( TWI_SR_DataSize > 0x01u )
			{
				/* send ACK */
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;
			}
			else
			{
				/* send NACK*/
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;

			}


			#if TWI_S_RECEIVE_GC_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_S_RECEIVE_GC_ACK_NOTIFICATION */

			break ;
		}

		case (TWI_ARBITRATION_LOST_GC) :
		{

			TWI_ModeBusy.MT_Mode = FALSE ;
			TWI_ModeBusy.MR_Mode = FALSE ;


			/* initialize data index */
			TWI_Internal_DataIndex = 0 ;

			/* check if slave will read one byte */
			if ( TWI_SR_DataSize > 0x01u )
			{
				/* send ACK */
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;
			}
			else
			{
				/* send NACK*/
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;

			}

			#if TWI_ARBITRATION_LOST_GC_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_ARBITRATION_LOST_GC_NOTIFICATION */

			break ;
		}

		case (TWI_S_RECEIVE_DATA_ACK) :
		{

			TWI_InternalSR_DataAddress[TWI_Internal_DataIndex] = (TWDR_REG->RegisterByte) ;
			TWI_Internal_DataIndex ++ ;

			if (TWI_Internal_DataIndex < TWI_SR_DataSize )
			{
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;
			}
			else
			{
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;
			}

			#if TWI_S_RECEIVE_DATA_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_S_RECEIVE_DATA_ACK_NOTIFICATION */

			break ;
		}

		case (TWI_S_RECEIVE_DATA_NACK) :
		{

			TWI_InternalSR_DataAddress[TWI_Internal_DataIndex] = (TWDR_REG->RegisterByte) ;

			(TWCR_REG->RegisterBits).Bit_6 = TWI_SlaveFlag ;
			TWI_ModeBusy.SR_Mode = FALSE ;


			#if TWI_S_RECEIVE_DATA_NACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_S_RECEIVE_DATA_NACK_NOTIFICATION */

			break ;
		}

		case (TWI_G_RECEIVE_DATA_ACK) :
		{


			TWI_InternalSR_DataAddress[TWI_Internal_DataIndex] = (TWDR_REG->RegisterByte) ;
			TWI_Internal_DataIndex ++ ;

			if (TWI_Internal_DataIndex < TWI_SR_DataSize )
			{
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;
			}
			else
			{
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;
			}

			#if TWI_G_RECEIVE_DATA_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_G_RECEIVE_DATA_ACK_NOTIFICATION */

			break ;
		}

		case (TWI_G_RECEIVE_DATA_NACK) :
		{


			TWI_InternalSR_DataAddress[TWI_Internal_DataIndex] = (TWDR_REG->RegisterByte) ;

			(TWCR_REG->RegisterBits).Bit_6 = TWI_SlaveFlag ;
			TWI_ModeBusy.SR_Mode = FALSE ;


			#if TWI_G_RECEIVE_DATA_NACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_G_RECEIVE_DATA_NACK_NOTIFICATION */

			break ;
		}

		case (TWI_RECEIVE_STOP_REPEATED) :
		{

			(TWCR_REG->RegisterBits).Bit_6 = TWI_SlaveFlag ;
			TWI_ModeBusy.SR_Mode = FALSE ;


			#if TWI_RECEIVE_STOP_REPEATED_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_RECEIVE_STOP_REPEATED_NOTIFICATION */

			break ;
		}

		case (TWI_RECEIVE_SLA_R_ACK) :
		{

			/* initialize data index */
			TWI_Internal_DataIndex = 0 ;

			(TWDR_REG->RegisterByte) =  TWI_InternalST_DataAddress[TWI_Internal_DataIndex] ;

			TWI_Internal_DataIndex ++ ;

			if ( TWI_Internal_DataIndex < TWI_ST_DataSize)
			{
				/* ACK should be received and new byte will be transmitted */
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;

			}
			else
			{
				/*  last byte NACK should be received and clear busy flag *
				 * if NACK don't received then TWSR will have TWI_S_TRANSMIT_LDATA_ACK */
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;

			}
			#if TWI_RECEIVE_SLA_R_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_RECEIVE_SLA_R_ACK_NOTIFICATION */


			break ;
		}
		case (TWI_ARBITRATION_LOST_SLA_R) :
		{


			TWI_ModeBusy.MT_Mode = FALSE ;
			TWI_ModeBusy.MR_Mode = FALSE ;

			TWI_Internal_DataIndex = 0 ;

			(TWDR_REG->RegisterByte) =  TWI_InternalST_DataAddress[TWI_Internal_DataIndex] ;

			TWI_Internal_DataIndex ++ ;

			if ( TWI_Internal_DataIndex < TWI_ST_DataSize)
			{
				/* ACK should be received and new byte will be transmitted */
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;

			}
			else
			{
				/*  last byte NACK should be received and clear busy flag *
				 * if NACK don't received then TWSR will have TWI_S_TRANSMIT_LDATA_ACK */
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;

			}


			#if TWI_ARBITRATION_LOST_SLA_R_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_ARBITRATION_LOST_SLA_R_NOTIFICATION */

			break ;
		}
		case (TWI_S_TRANSMIT_DATA_ACK) :
		{

			(TWDR_REG->RegisterByte) =  TWI_InternalST_DataAddress[TWI_Internal_DataIndex] ;

			TWI_Internal_DataIndex ++ ;

			if ( TWI_Internal_DataIndex < TWI_ST_DataSize)
			{
				/* ACK should be received and new byte will be transmitted */
				(TWCR_REG->RegisterBits).Bit_6 = TRUE ;

			}
			else
			{
				/*  last byte NACK should be received and clear busy flag *
				 * if NACK don't received then TWSR will have TWI_S_TRANSMIT_LDATA_ACK */
				(TWCR_REG->RegisterBits).Bit_6 = FALSE ;

			}

			#if TWI_S_TRANSMIT_DATA_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_S_TRANSMIT_DATA_ACK_NOTIFICATION */

			break ;
		}
		case (TWI_S_TRANSMIT_DATA_NACK) :
		{

			TWI_ModeBusy.ST_Mode = FALSE ;
			(TWCR_REG->RegisterBits).Bit_6 = TWI_SlaveFlag ;


			#if TWI_S_TRANSMIT_DATA_NACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* TWI_S_TRANSMIT_DATA_NACK_NOTIFICATION */

			break ;
		}
		case (TWI_S_TRANSMIT_LDATA_ACK) :
		{


			TWI_ModeBusy.ST_Mode = FALSE ;
			(TWCR_REG->RegisterBits).Bit_6 = TWI_SlaveFlag ;


			#if WI_S_TRANSMIT_LDATA_ACK_NOTIFICATION == TRUE

			TWI_NotificationFunction () ;

			#endif /* WI_S_TRANSMIT_LDATA_ACK_NOTIFICATION */

			break ;
		}
		case (TWI_NO_REVELANT_STATE) :
		{

			break ;
		}

	}

	/* clear TWINT */
	(TWCR_REG->RegisterBits).Bit_7 = TRUE ;



	return ;
}


/***********************************************************************************/


