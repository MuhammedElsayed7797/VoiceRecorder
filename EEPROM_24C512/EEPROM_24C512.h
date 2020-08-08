/*
 * EEPROM_24C512.h
 *
 * Created on: Mar 18, 2020
 * Author: Bebo
 *
 */

#ifndef EEPROM_24C512_EEPROM_24C512_H_
#define EEPROM_24C512_EEPROM_24C512_H_

#include "../TWI/TWI.h"


/*****************************************************************************/

#ifndef EEPROM_24C512_SOURCE_GAURD

/* if true then current EEPROM operation not done and cannot call anoter EEPROM function */
extern volatile boolean EE24C512_BusyFlag  ;


#endif

/***********************************************************************************/

/* used to configure notification parameters dataTypes */
#define EE24C512_NOTIFICATION_DATA_TYPE	uint8


/* IMPORTANT NOTE :
 *
 * user mustn't modify Notification function of  TWI peripheral ISR between
 * calling EEPROM READ/WRITE functions and EEPROM notification which indicate EERPROM operation is finished
 *
 * READ/WRITE EEPROM function must be called when device is virtually disconnected from TWI bus
 * which mean device don't wait and won't respond on its address on TWI bus
 * and won't wait to transmit START to become master
 *
 * as example after deinit and init TWI
 * or after end of master mode transmission and slave mode is disabled
 *
 * for READ/WRITE function work correctly TWI_TRANSMIT_SLA_R_NACK_NOTIFICATION, TWI_TRANSMIT_SLA_W_NACK_NOTIFICATION
 * must be TRUE
 * and for notification when  READ/WRITE with EEPROM complete
 * TWI_TRANSMIT_DATA_ACK_NOTIFICATION,TWI_M_RECEIVE_DATA_NACK_NOTIFICATION must be TRUE
 *
 */


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
 * TransmittedData is the address of first byte in this array  *
 * DataSize determine size of this array
 *
 * this function is could be used to initiate internal address of EEPROM to Sequential Read operation
 * start form Byte_Address
 *
 * */

StatusType EE24C512_Write ( uint8 EEPROM_Number, uint16* Byte_Address, uint8* TransmittedData, uint8 DataSize ) ;

/********************************************************************************************************/

/* used to read EEPROM memory
 * on the same bus max number of 24C512 memory is 8 so EEPROM_Number has number of memory to
 * talk with according to its A2,A1,A0 pins  from 000 ~ 111 if EEPROM_Number = 6
 * then it will send to memory which its A0 is low and A1,A2 is hihg
 *
 * Byte_Address refer to first location byte inside the memory to write in
 *
 * user must has array of bytes to be receive data from EEPROM this array can't be modified
 * before EE24C512_Write is complete  except for  user certain purpose
 * ReceivedData is the address of first byte in this array  *
 * DataSize determine size of this array
 *
 * this function is could be used to read from internal address of EEPROM without need to send
 * a new address if Byte_Address is equal to NULL_PTR
 *
 * */


StatusType EE24C512_Read ( uint8 EEPROM_Number, uint16* Byte_Address, uint8* ReceivedData, uint8 DataSize ) ;

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

StatusType EE24C512_Notification ( EE24C512_NOTIFICATION_DATA_TYPE * DestinationAddress, EE24C512_NOTIFICATION_DATA_TYPE DestinationValue, EE24C512_NOTIFICATION_DATA_TYPE ValueMask ) ;


/********************************************************************************************************/



#endif /* EEPROM_24C512_EEPROM_24C512_H_ */
