/*
 * 	TWI_Cfg.h
 *
 *  Created on: Mar 12, 2020
 *  Author: Bebo
 *
 */

#ifndef TWI_TWI_CFG_H_
#define TWI_TWI_CFG_H_


/***********************************************************************************/


/*determine device address on TWI bus consist of 7 bits (least 7 bits )*/
#define TWI_DEVICE_ADDRESS		(0x55)

/* determine BIT_RATE used in SCL frequency equation */
#define TWI_BIT_RATE			(6u)

/* determine prescaler used in SCL frequency equation */
#define TWI_PRESCALER			(0b01)

/***********************************************************************************/

/* notification configuration */
/*
 * TWI has different 27 state every state generate a TWI interrupt
 * cause TWI ISR, user can provide and notification function that will be
 * called when enabled state happen
 *
 * below configuration will determine which states will be enabled
 * and will cause call of notifacation function inside TWI ISR
 *
 */




#define TWI_ERROR_NOTIFICATION								(FALSE)
#define TWI_TRANSMIT_START_NOTIFICATION						(FALSE)
#define TWI_TRANSMIT_REPEATED_NOTIFICATION					(FALSE)
#define TWI_ARBITRATION_LOST_NOTIFICATION					(FALSE)
#define TWI_NO_REVELANT_STATE_NOTIFICATION					(FALSE)


#define TWI_TRANSMIT_SLA_W_ACK_NOTIFICATION					(FALSE)
#define TWI_TRANSMIT_SLA_W_NACK_NOTIFICATION				(TRUE)
#define TWI_TRANSMIT_DATA_ACK_NOTIFICATION					(TRUE)
#define TWI_TRANSMIT_DATA_NACK_NOTIFICATION					(FALSE)

#define TWI_TRANSMIT_SLA_R_ACK_NOTIFICATION					(FALSE)
#define TWI_TRANSMIT_SLA_R_NACK_NOTIFICATION				(TRUE)
#define TWI_M_RECEIVE_DATA_ACK_NOTIFICATION					(FALSE)
#define TWI_M_RECEIVE_DATA_NACK_NOTIFICATION				(TRUE)

#define TWI_RECEIVE_SLA_W_ACK_NOTIFICATION					(FALSE)
#define TWI_ARBITRATION_LOST_SLA_W_NOTIFICATION				(FALSE)
#define TWI_S_RECEIVE_GC_ACK_NOTIFICATION					(FALSE)
#define TWI_ARBITRATION_LOST_GC_NOTIFICATION				(FALSE)
#define TWI_S_RECEIVE_DATA_ACK_NOTIFICATION					(FALSE)
#define TWI_S_RECEIVE_DATA_NACK_NOTIFICATION				(FALSE)
#define TWI_G_RECEIVE_DATA_ACK_NOTIFICATION					(FALSE)
#define TWI_G_RECEIVE_DATA_NACK_NOTIFICATION				(FALSE)
#define TWI_RECEIVE_STOP_REPEATED_NOTIFICATION				(FALSE)


#define TWI_RECEIVE_SLA_R_ACK_NOTIFICATION					(FALSE)
#define TWI_ARBITRATION_LOST_SLA_R_NOTIFICATION				(FALSE)
#define TWI_S_TRANSMIT_DATA_ACK_NOTIFICATION				(FALSE)
#define TWI_S_TRANSMIT_DATA_NACK_NOTIFICATION				(FALSE)
#define WI_S_TRANSMIT_LDATA_ACK_NOTIFICATION				(FALSE)





/***********************************************************************************/


#endif /* TWI_TWI_CFG_H_ */
