/****************************************************************************************
 *	 File Name: RTC.h																	*
 * 	 Reference: https://sysplay.in/blog/tag/real-time-clock-interfacing-with-atmega16/	*
 *  Created on: Oct 19, 2018															*
 *      Author: Khalid Tarek															*
 ****************************************************************************************/

#ifndef RTC_H_
#define RTC_H_

#include"std_types.h"

uint8 day_search(char day[]);
void day_get(const char str[], char sep, char day[]);

void rtc_init(void);
uint8 rtc_set(uint8 y, uint8 mo, uint8 d, uint8 dy, uint8 h, uint8 m,
		uint8 s);
uint8 rtc_set_time(char *time_str);
uint8 rtc_set_date(char *date_str);
uint8 rtc_get(uint8 *y, uint8 *mo, uint8 *d, uint8 *dy,
		uint8 *h, uint8 *m, uint8 *s);
uint8 rtc_get_str(char date_str[15], char time_str[9]);

#endif /* RTC_H_ */
