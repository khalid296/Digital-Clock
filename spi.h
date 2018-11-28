 /******************************************************************************
 *
 * Module: SPI
 *
 * File Name: spi.h
 *
 * Description: Header file for the SPI AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef SPI_H_
#define SPI_H_

#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

#define SPI_PORT_DIR DDRB
#define SPI_PORT_PORT PORTB
#define SS PB4
#define MOSI PB5
#define MISO PB6
#define SCK PB7

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void SPI_initMaster(void); 
void SPI_initSlave(void);
uint8 SPI_TrancieveByte(const uint8 data);
void SPI_sendByte(const uint8 data);
uint8 SPI_recieveByte(void);
void SPI_sendString(const uint8 *Str);
void SPI_receiveString(char *Str);

#endif
