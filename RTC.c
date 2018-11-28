/****************************************************************************************
 *	 File Name: RTC.c																	*
 * 	 Reference: https://sysplay.in/blog/tag/real-time-clock-interfacing-with-atmega16/	*
 *  Created on: Oct 19, 2018															*
 *      Author: Khalid Tarek															*
 ****************************************************************************************/

#include "twi.h"
#include "rtc.h"

#define RTC_DEV_ADDR 0b1101000


uint8 day_search(char day[]){
	uint8 i;
	char *day_str[] = { "", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	/* what is i? the number of items used in the array? */
	for(i = 0; i < 8; i++) {
		if (strcmp(day, day_str[i])==0){
			/* match, x is the index */
			return i;
		}
	}
	/* here with no match */
	return 0;
}

void day_get(const char str[], char sep, char day[])
{
	uint8 i=0,c=0,position,length=3;
	while(str[i] != '\0')
	{
		if (str[i] == sep) {
			position=i+1;
			break;
		}
		i++;
	}
	while (c < length) {
		day[c] = str[position+c];
		c++;
	}
	day[c]='\0';
}


void rtc_init(void)
{
	TWI_ConfigType  TWI_Config = {VAL_1,32768,0x01};
	TWI_init(&TWI_Config);
}

uint8 rtc_set_time(char *time_str)
{
	uint8 rtc_data[4] = { /* RTC Registers' start address */ 0x00, };
	uint8 *rtc_reg = rtc_data + 1;

	/* RTC Registers */
	
	rtc_reg[0] = ((time_str[6] - '0') << 4) | ((time_str[7] - '0') & 0xF);
	rtc_reg[1] = ((time_str[3] - '0') << 4) | ((time_str[4] - '0') & 0xF);
	rtc_reg[2] = ((time_str[0] - '0') << 4) | ((time_str[1] - '0') & 0xF);
	
	return TWI_master_tx(RTC_DEV_ADDR, rtc_data, sizeof(rtc_data));
}

uint8 rtc_set_date(char *date_str)
{
	uint8 rtc_data[5] = { /* RTC Registers' start address */ 0x03, };
	uint8 *rtc_reg = rtc_data + 1;
	uint8 day;
	char day_str[5];

	/* RTC Registers */
	day_get(date_str,' ',day_str);
	day = day_search(day_str);

	rtc_reg[0]= day & 0xF;

	rtc_reg[1] = ((date_str[0] - '0') << 4) | ((date_str[1] - '0') & 0xF);
	rtc_reg[2] = ((date_str[3] - '0') << 4) | ((date_str[4] - '0') & 0xF);
	rtc_reg[3] = ((date_str[8] - '0') << 4) | ((date_str[9] - '0') & 0xF);

	return TWI_master_tx(RTC_DEV_ADDR, rtc_data, sizeof(rtc_data));
}

uint8 rtc_set(uint8 y, uint8 mo, uint8 d, uint8 dy, uint8 h, uint8 m,
		uint8 s)
{
	uint8 rtc_data[8] = { /* RTC Registers' start address */ 0x00, };
	uint8 *rtc_reg = rtc_data + 1;

	/* RTC Registers */
	rtc_reg[0] = ((s / 10) << 4) | (s % 10); /* seconds in BCD */
	rtc_reg[1] = ((m / 10) << 4) | (m % 10); /* minutes in BCD */
	rtc_reg[2] = ((h / 10) << 4) | (h % 10); /* hours in BCD */
	rtc_reg[3] = dy; /* day in BCD : 7 for Saturday */
	rtc_reg[4] = ((d / 10) << 4) | (d % 10); /* date in BCD */
	rtc_reg[5] = ((mo / 10) << 4) | (mo % 10); /* month in BCD */
	rtc_reg[6] = ((y / 10) << 4) | (y % 10); /* year in BCD */
	rtc_reg[7] = 0x13;

	return TWI_master_tx(RTC_DEV_ADDR, rtc_data, sizeof(rtc_data));
}

uint8 rtc_get(uint8 *y, uint8 *mo, uint8 *d, uint8 *dy,
		uint8 *h, uint8 *m, uint8 *s)
{
	int ret;
	uint8 rtc_data = 0x00; /* RTC Registers' start address */
	uint8 rtc_reg[7];

	if ((ret = TWI_master_tx_rx(RTC_DEV_ADDR, &rtc_data, 1, rtc_reg, 7)) == 0)
	{
		/* seconds from BCD */
		*s = 10 * ((rtc_reg[0] >> 4) & 0x7) + (rtc_reg[0] & 0xF);
		/* minutes from BCD */
		*m = 10 * (rtc_reg[1] >> 4) + (rtc_reg[1] & 0xF);
		/* hours from BCD */
		*h = 10 * (rtc_reg[2] >> 4) + (rtc_reg[2] & 0xF);
		/* day from BCD : 7 for Saturday */
		*dy = rtc_reg[3];
		/* date from BCD */
		*d = 10 * (rtc_reg[4] >> 4) + (rtc_reg[4] & 0xF);
		/* month from BCD */
		*mo = 10 * (rtc_reg[5] >> 4) + (rtc_reg[5] & 0xF);
		/* year from BCD */
		*y = 10 * (rtc_reg[6] >> 4) + (rtc_reg[6] & 0xF);
	}
	return ret;
}

uint8 rtc_get_str(char date_str[15], char time_str[9])
/* Format Example: date_str[] = "01.06.2075 Sat"; time_str[] = "03:07:00"; */
{
	uint8 ret;
	uint8 rtc_data = 0x00; /* RTC Registers' start address */
	uint8 rtc_reg[7];
	uint8 day;
	char *day_str[] = { "", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

	if ((ret = TWI_master_tx_rx(RTC_DEV_ADDR, &rtc_data, 1, rtc_reg, 7)) == 0)
	{
		date_str[0] = '0' + (rtc_reg[4] >> 4);
		date_str[1] = '0' + (rtc_reg[4] & 0xF);
		date_str[2] = '.';
		date_str[3] = '0' + (rtc_reg[5] >> 4);
		date_str[4] = '0' + (rtc_reg[5] & 0xF);
		date_str[5] = '.';
		date_str[6] = '2';
		date_str[7] = '0';
		date_str[8] = '0' + (rtc_reg[6] >> 4);
		date_str[9] = '0' + (rtc_reg[6] & 0xF);
		date_str[10] = ' ';

		day = rtc_reg[3] & 0xF;
		date_str[11] = day_str[day][0];
		date_str[12] = day_str[day][1];
		date_str[13] = day_str[day][2];
		date_str[14] = 0;

		time_str[0] = '0' + (rtc_reg[2] >> 4);
		time_str[1] = '0' + (rtc_reg[2] & 0xF);
		time_str[2] = ':';
		time_str[3] = '0' + (rtc_reg[1] >> 4);
		time_str[4] = '0' + (rtc_reg[1] & 0xF);
		time_str[5] = ':';
		time_str[6] = '0' + ((rtc_reg[0] >> 4) & 0x7);
		time_str[7] = '0' + (rtc_reg[0] & 0xF);
		time_str[8] = 0;
	}
	return ret;
}


void stop_RTC(){
	TWI_master_tx(RTC_DEV_ADDR, 0x80, sizeof(1));
}
