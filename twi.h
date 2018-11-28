/************************************************************************************************
 *	 File Name: twi.h																			*
 * 	 Reference: https://sysplay.in/blog/diy/electronics/2016/03/programming-the-i2c-protocol/ 	*
 *  Created on: Oct 19, 2018																	*
 *      Author: Khalid Tarek																	*
 ************************************************************************************************/
#ifndef TWI_H_
#define TWI_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

#define TWI_PORT_DIR DDRC
#define TWI_PORT_PORT PORTC
#define SCL PC0
#define SDA PC1

/* TWI Master Status Codes */
#define	TW_START 			0x08	// start has been sent
#define	TW_REP_START 		0x10	// repeated start
#define	TW_MT_SLA_W_ACK 	0x18	// Master transmit ( slave address + Write request ) to slave + Ack received from slave
#define	TW_MT_SLA_W_NOACK 	0x20
#define	TW_MT_DATA_ACK 		0x28	// Master transmit data and ACK has been received from Slave.
#define	TW_MT_DATA_NOACK 	0x30
#define	TW_ARB_LOST 		0x38	//Arbitrary lost
#define	TW_MT_SLA_R_ACK 	0x40	// Master transmit ( slave address + Read request ) to slave + Ack received from slave
#define	TW_MT_SLA_R_NOACK 	0x48
#define	TW_MR_DATA_ACK		0x50	// Master received data and send ACK to slave
#define	TW_MR_DATA_NOACK 	0x58	// Master received data but doesn't send ACK to slave

#define DIR_WRITE 0
#define DIR_READ 1

#define QUIT_TWI_OP { send_stop(); return -1; }
#define TWBR_VAL(SCL,TWPS_val) (((F_CPU/SCL)-16UL)/(2*TWPS_val))

typedef enum
{
	VAL_1, VAL_4, VAL_16, VAL_64
}TWI_Prescaler;

typedef struct
{
	TWI_Prescaler prescaler;
	uint8 frequency;
	uint8 slave_address;
}TWI_ConfigType;

void TWI_init();
void TWI_shut(void);
uint8 TWI_master_tx(uint8 addr, uint8 *data, uint16 len);
uint8 TWI_master_rx(uint8 addr, uint8 *data, uint16 len);
uint8 TWI_master_tx_rx(uint8 addr, uint8 *tx_data, uint16 tx_len, uint8 *rx_data,
			uint16 rx_len);

#endif /* TWI_H_ */
