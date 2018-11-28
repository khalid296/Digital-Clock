/************************************************************************************************
 *	 File Name: twi.c																			*
 * 	 Reference: https://sysplay.in/blog/diy/electronics/2016/03/programming-the-i2c-protocol/ 	*
 *  Created on: Oct 19, 2018																	*
 *      Author: Khalid Tarek																	*
 ************************************************************************************************/

#include "twi.h"

static uint8 Pow(uint8 number, uint8 power){
	uint8 i, result=1;

	for(i=0; i<power; i++){
		result*=number;
	}
	return result;
}

void TWI_init(const TWI_ConfigType * Config_Ptr)
{
	uint8 twps_value;
	// 1 = output, 0 = input
	TWI_PORT_DIR  &= ~0x03; // PC0 = SCL; PC1 = SDA
	TWI_PORT_PORT |= 0x03; // Internal pull-up on both lines

	twps_value = Pow(4,Config_Ptr->prescaler);
	TWBR = TWBR_VAL(Config_Ptr->frequency,twps_value);

	TWSR = (TWSR & 0xFC) | (Config_Ptr->prescaler);  // Clearing TWSP to 0

	TWAR = (Config_Ptr->slave_address << 1);

	SET_BIT(TWCR,TWEN); // Enable TWI, generating the SCLK
}

void TWI_shut(void)
{
	// 1 = output, 0 = input
	TWI_PORT_DIR &= ~0x03; // PC0 = SCL; PC1 = SDA
	TWI_PORT_PORT &= ~0x03; // Clear pull-up on both lines
	//Clear TWI registers
	TWBR = 0;
	TWSR = 0;
	TWAR = 0;
	TWCR = 0;
}

static uint8 get_status(uint8 status)
{
	while(BIT_IS_CLEAR(TWCR,TWINT));

	if ((TWSR & 0xF8) == status)
		return 0;
	else
		return TWSR & 0xF8;
}

static uint8 send_start(uint8 status)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	return get_status(status);
}

static void send_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

static uint8 send_data(uint8 data, uint8 status)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	return get_status(status);
}

static uint8 recv_data(uint8 *data, uint8 status, uint8 ack)
{
	TWCR = (1 << TWINT) | (ack << TWEA) | (1 << TWEN);
	if (get_status(status) == 0)
	{
		*data = TWDR;
		return 0;
	}
	else
	{
		return -1;
	}
}

uint8 TWI_master_tx(uint8 addr, uint8 *data, uint16 len)
{
	uint8 i;

	if (send_start(TW_START)) QUIT_TWI_OP;
	if (send_data((addr << 1) | DIR_WRITE, TW_MT_SLA_W_ACK)) QUIT_TWI_OP;
	for (i = 0; i < len; i++)
	{
		if (send_data(data[i], TW_MT_DATA_ACK)) QUIT_TWI_OP;
	}
	send_stop();
	return 0;
}
uint8 TWI_master_rx(uint8 addr, uint8 *data, uint16 len)
{
	uint8 i;

	if (send_start(TW_START)) QUIT_TWI_OP;
	if (send_data((addr << 1) | DIR_READ, TW_MT_SLA_R_ACK)) QUIT_TWI_OP;
	for (i = 0; i < len - 1; i++)
	{
		if (recv_data(&data[i], TW_MR_DATA_ACK, 1)) QUIT_TWI_OP;
	}
	if (recv_data(&data[i], TW_MR_DATA_NOACK, 0)) QUIT_TWI_OP;
	send_stop();
	return 0;
}
uint8 TWI_master_tx_rx(uint8 addr, uint8 *tx_data, uint16 tx_len, uint8 *rx_data,
			uint16 rx_len)
{
	uint8 i;

	if (send_start(TW_START)) QUIT_TWI_OP;
	if (send_data((addr << 1) | DIR_WRITE, TW_MT_SLA_W_ACK)) QUIT_TWI_OP;
	for (i = 0; i < tx_len; i++)
	{
		if (send_data(tx_data[i], TW_MT_DATA_ACK)) QUIT_TWI_OP;
	}
	if (send_start(TW_REP_START)) QUIT_TWI_OP;
	if (send_data((addr << 1) | DIR_READ, TW_MT_SLA_R_ACK)) QUIT_TWI_OP;
	for (i = 0; i < rx_len - 1; i++)
	{
		if (recv_data(&rx_data[i], TW_MR_DATA_ACK, 1)) QUIT_TWI_OP;
	}
	if (recv_data(&rx_data[i], TW_MR_DATA_NOACK, 0)) QUIT_TWI_OP;
	send_stop();
	return 0;
}
