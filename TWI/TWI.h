/*
 * 	TWI.h
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 *
 */


#ifndef TWI_TWI_H_
#define TWI_TWI_H_

#include "../Common/Utils.h"
#include "TWI_Cfg.h"



/***********************************************************************************/



/* TWI had 27 different state can represented on TWSR register */

#define TWI_ERROR_STATE						(0x00u)
#define TWI_TRANSMIT_START					(0x08u)
#define TWI_TRANSMIT_REPEATED				(0x10u)
#define TWI_ARBITRATION_LOST				(0x38u)
#define TWI_NO_REVELANT_STATE				(0xF8u)


#define TWI_TRANSMIT_SLA_W_ACK				(0x18u)
#define TWI_TRANSMIT_SLA_W_NACK				(0x20u)
#define TWI_TRANSMIT_DATA_ACK				(0x28u)
#define TWI_TRANSMIT_DATA_NACK				(0x30u)

#define TWI_TRANSMIT_SLA_R_ACK				(0x40u)
#define TWI_TRANSMIT_SLA_R_NACK				(0x48u)
#define TWI_M_RECEIVE_DATA_ACK				(0x50u)
#define TWI_M_RECEIVE_DATA_NACK				(0x58u)

#define TWI_RECEIVE_SLA_W_ACK				(0x60u)
#define TWI_ARBITRATION_LOST_SLA_W			(0x68u)
#define TWI_S_RECEIVE_GC_ACK				(0x70u)
#define TWI_ARBITRATION_LOST_GC				(0x78u)
#define TWI_S_RECEIVE_DATA_ACK				(0x80u)
#define TWI_S_RECEIVE_DATA_NACK				(0x88u)
#define TWI_G_RECEIVE_DATA_ACK				(0x90u)
#define TWI_G_RECEIVE_DATA_NACK				(0x98u)
#define TWI_RECEIVE_STOP_REPEATED			(0xA0u)



#define TWI_RECEIVE_SLA_R_ACK				(0xA8u)
#define TWI_ARBITRATION_LOST_SLA_R			(0xB0u)
#define TWI_S_TRANSMIT_DATA_ACK				(0xB8u)
#define TWI_S_TRANSMIT_DATA_NACK			(0xC0u)
#define TWI_S_TRANSMIT_LDATA_ACK			(0xC8u)



/*****************************************************************************/

#ifndef TWI_SOURCE_GAURD

/* used inside notification function to know number of byte transmitted or received */
extern volatile uint8 TWI_Internal_DataIndex  ;


/* used to know state of TWI when its interrupt flag is set and wait for response form application
 * this variable has its value from TWSR register and using it outside Notification function is meaningless
 */
extern volatile uint8 TWI_CurrentState  ;

/* those will be assigned to function which will be called before and after and during data transmission, and in case of error */
extern void (* TWI_NotificationFunction ) (void)  ;


#endif

/***********************************************************************************/



/* when called enable TWI hardware & interrupt and initiate its settings
 * used to initialize SCL frequency  of TWI according to equation of
 * CPU_CLOCK/(16 + ( 2 * TWI_BIT_RATE * 2^(TWI_PRESCALER) ) )
 * max SCL frequency is 400KHz
 * also used to initiate 7 bit address of device according to lowest 7 bits in TWI_DEVICE_ADDRESS
 * */

void TWI_Init (void) ;


/***********************************************************************************/

/*
 * disable TWI hardware and interrupt
 * */
void TWI_DeInit (void) ;


/***********************************************************************************/

/*
 * this function called when device want to become a master and transmit data
 * user must have an array of data to be transmitted
 * TWI_MT_SR_DataSize must hold array size before calling this function
 * TransmittedData hold address of first byte to be transmitted
 * SlaveAddress hold the address of slave device to receive data
 *
 * Note: in case of this function is called before MT or MR previous transmission requested and waiting
 * or didn't finished yet then it will return E_NOT_OK
 *
 * array of data to be transmitted mustn't be received until transmission is finished
 * except user was intend to modify it for some reason
 *
 * its recommended to call this function in Notification function under certain user condition,
 * indicating that current transmission is ended
 * to avoid calling it while another transmission operation processed
 * and to avoid pulling over E_OK status type
 *
 * */
StatusType TWI_MT ( uint8 * TransmittedData, uint8  SlaveAddress, uint8 DataSize ) ;


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

StatusType TWI_MR ( uint8 * ReceivedData, uint8  SlaveAddress, uint8 DataSize ) ;


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
StatusType TWI_ST ( uint8 * TransmittedData, uint8 DataSize ) ;


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
StatusType TWI_SR ( uint8 * ReceivedData, uint8 DataSize ) ;


/***********************************************************************************/

/*
 * this API used to disable or enable general call recognition
 * according to value of Enable_GC, if TRUE -> enable
 * */

void TWI_GeneralCall ( boolean Enable_GC ) ;


/***********************************************************************************/


/* used to generate STOP on the bus in case of master
 * adn to be non addressed slave in case of slave
 * */
void TWI_TransmissionSTOP ( void ) ;



/***********************************************************************************/


/* used to virtually disconnect device from bus
 * device won't respond with ACK when its address recived on bus */

void TWI_UnSlave (void) ;


/***********************************************************************************/

#endif /* TWI_TWI_H_ */
