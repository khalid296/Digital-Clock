/************************************************
 *	 File Name: Digital_Clock.h					*
 * Description: 								*
 *  Created on: Oct 25, 2018							*
 *      Author: Khalid Tarek					*
 ************************************************/
#ifndef DIGITAL_CLOCK_H_
#define DIGITAL_CLOCK_H_

#include"std_types.h"
#include"micro_config.h"
#include"common_macros.h"
#include"lcd.h"
#include"spi.h"
#include"twi.h"
#include"RTC.h"
#include <string.h>
#include <stdio.h>


#define BUTTON_CTRL_PORT_DIR DDRB
#define BUTTON_CTRL_PORT_OUT PORTB
#define BUTTON_CTRL_PORT_IN PINB
#define BUTTON1 PB2
#define BUTTON2 PB1
#define BUTTON3 PB0
#define BUTTON4 PB3

#define SS PB4

#define BUZZER_CTRL_PORT_DIR DDRC
#define BUZZER_CTRL_PORT_OUT PORTC
#define BUZZER PC2

#define DEBOUNCE_DELAY 5
#define ALARM_SIZE 3
#define ALARM_FLAG_ADDRESS 0x01
#define ALARM_START_ADDRESS 0x02

void clock_init();
void temp_init();
uint8 get_temp();
void LCD_display_update();
void set_alarm(char *alarm_str);
void get_alarm(char alarm_str[10]);
void options();
#endif /* DIGITAL_CLOCK_H_ */
