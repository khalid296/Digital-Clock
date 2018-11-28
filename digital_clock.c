/************************************************
 *	 File Name: digital_clock.c					*
 * Description: 								*
 *  Created on: Oct 19, 2018					*
 *      Author: Khalid Tarek					*
 ************************************************/

#include"Digital_Clock.h"

#define TIME 1
#define DATE 2
#define ALARM 3
#define STR_SIZE 15

//char g_date[] = "01.06.2075 Sat";
//char g_time[] = "03:07:00";
uint8 g_alarm_status = ON;
char g_alarm_compare[]="00:00 sat";
char *clock[STR_SIZE]= {"","03:07:00","01.06.2075 Sat", "00:00 sat"};
uint8 g_page = 0;

//static uint8 Pow(uint8 number, uint8 power){
//	uint8 i, result=1;
//
//	for(i=0; i<power; i++){
//		result*=number;
//	}
//	return result;
//}

uint8 button_is_pressed (volatile uint8 *register_name, uint8 pin_number)
{
	uint8 flag=RELEASED;
	if(BIT_IS_CLEAR(*register_name, pin_number)){
		_delay_ms(DEBOUNCE_DELAY);
		//Second check due to switch bouncing
		if(BIT_IS_CLEAR(*register_name, pin_number))
		{
			flag=PRESSED;
		}
		//Hold until button is not pressed
		while(BIT_IS_CLEAR(*register_name, pin_number)){}
	}
	return flag;
}

static void Internal_EEPROM_write(uint8 uiAddress, uint8 ucData)
{
	//	1. Wait until EEWE becomes zero.
	//	2. Wait until SPMEN in SPMCR becomes zero.
	//	3. Write new EEPROM address to EEAR (optional).
	//	4. Write new EEPROM data to EEDR (optional).
	//	5. Write a logical one to the EEMWE bit while writing a zero to EEWE in EECR.
	//	6. Within four clock cycles after setting EEMWE, write a logical one to EEWE.

	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE))
	;
	/* Set up address and data registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMWE */
	EECR |= (1<<EEMWE);
	/* Start eeprom write by setting EEWE */
	EECR |= (1<<EEWE);
}

static uint8 Internal_EEPROM_read(uint8 uiAddress)
{
/* Wait for completion of previous write */
while(EECR & (1<<EEWE))
;
/* Set up address register */
EEAR = uiAddress;
/* Start eeprom read by writing EERE */
EECR |= (1<<EERE);
/* Return data from data register */
return EEDR;
}

void temp_init(){
	SET_BIT(PORTB,SS);
	_delay_us(1);
	SPI_TrancieveByte(0x80);
	_delay_us(1);
	SPI_TrancieveByte(0x00);
	_delay_us(1);
	CLEAR_BIT(PORTB,SS);

}

uint8 get_temp(){
	uint8 temp=0;
	//read temp from TC72
	SET_BIT(PORTB,SS);
	_delay_us(1);
	SPI_TrancieveByte(0x02);
	_delay_us(1);
	temp = SPI_TrancieveByte(0x00);
	_delay_us(1);
	CLEAR_BIT(PORTB,SS);

	return temp;
}

void LCD_display_update(){
	uint8 i;
	if (rtc_get_str(clock[DATE], clock[TIME]) == 0)
	{
		LCD_displayStringRowColumn(1,0,clock[DATE]);

		LCD_displayStringRowColumn(2,0,clock[TIME]);

		LCD_goToRowColumn(2,10);
		LCD_intgerToString(get_temp());
		LCD_displayStringRowColumn(2,15,"C");
		for(i=0;i<5;i++){
			g_alarm_compare[i] = clock[TIME][i];
		}
		g_alarm_compare[5] = ' ';
		for(i=11;i<14;i++){
			g_alarm_compare[i-5] = clock[DATE][i];
		}
	}
	else
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(1,0,"RTC set failed.");
	}
}

void set_alarm(char *alarm_str){
	uint8 rtc_reg[3], day, i;

	char day_str[5];
	

	rtc_reg[0] = ((alarm_str[0] - '0') << 4) | ((alarm_str[1] - '0') & 0xF);
	rtc_reg[1] = ((alarm_str[3] - '0') << 4) | ((alarm_str[4] - '0') & 0xF);
	
	day_get(alarm_str,' ',day_str);
	day = day_search(day_str);

	rtc_reg[2]= day & 0xF;

	Internal_EEPROM_write(ALARM_FLAG_ADDRESS, ON);

	for(i=0;i<ALARM_SIZE;i++){
		Internal_EEPROM_write(ALARM_START_ADDRESS+i, rtc_reg[i]);
	}


}

void get_alarm(char alarm_str[]){
	uint8 i, rtc_reg[3], day;
	char *day_str[] = { "", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

	if(g_alarm_status == ON){
	for(i=0;i<ALARM_SIZE;i++){
		rtc_reg[i]= Internal_EEPROM_read(ALARM_START_ADDRESS+i);
	}
		alarm_str[0] = '0' + (rtc_reg[0] >> 4);
		alarm_str[1] = '0' + (rtc_reg[0] & 0xF);
		alarm_str[2] = ':';
		alarm_str[3] = '0' + (rtc_reg[1] >> 4);
		alarm_str[4] = '0' + (rtc_reg[1] & 0xF);

		alarm_str[5] = ' ';

		day = rtc_reg[2] & 0xF;
		alarm_str[6] = day_str[day][0];
		alarm_str[7] = day_str[day][1];
		alarm_str[8] = day_str[day][2];
		alarm_str[9] = 0;

	}
}

uint8 check_alarm(){
	if(strcmp(clock[ALARM], g_alarm_compare)==0)
		return ON;
	else
		return OFF;
}

void alarm(){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,7,"Alarm");
	get_alarm(clock[ALARM]);
	LCD_displayStringRowColumn(2,0,clock[ALARM]);
	_delay_ms(1000);
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,1,"Digital Clock");
	LCD_displayStringRowColumn(3,0,"Options");
	LCD_displayStringRowColumn(3,10,"Alarm");

}
void clock_init(){
	rtc_init();
	LCD_init();/*initialize lcd*/
	SPI_initMaster();
	temp_init();

	g_alarm_status = Internal_EEPROM_read(0x01);
	get_alarm(clock[ALARM]);

	BUTTON_CTRL_PORT_DIR &=0xF0;	//Configure first 5 pins of Port A as input for 4 Push buttons and start

	BUTTON_CTRL_PORT_OUT |=0x0F;	//Enable internal pull up resistors for buttons
	
	SET_BIT(BUZZER_CTRL_PORT_DIR,BUZZER);
	CLEAR_BIT(BUZZER_CTRL_PORT_OUT,BUZZER);

	LCD_displayStringRowColumn(0,0,"Digital Clock");/*Display calculator in the first line*/

	/* Initialize the clock to 2075.June.1st Sat 03:07:00 */
	/*
	if (rtc_set(75, 6, 1, 7, 3, 7, 0) == 0)
	{
		LCD_displayStringRowColumn(1,0,clock[DATE]);

		LCD_displayStringRowColumn(2,0,clock[TIME]);

		LCD_goToRowColumn(2,10);
		LCD_intgerToString(get_temp());
		LCD_displayStringRowColumn(2,15,"C");

		LCD_displayStringRowColumn(3,0,"Options");
		LCD_displayStringRowColumn(3,10,"Alarm");
	}
	else
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(1,0,"RTC set failed.");
		while (1);
	}
	*/
	LCD_display_update();
	LCD_displayStringRowColumn(3,0,"Options");
	LCD_displayStringRowColumn(3,10,"Alarm");
}

void edit(uint8 option, uint8 position){
	char option_str_copy[STR_SIZE];
	strcpy(option_str_copy,clock[option]);
	char *options[4][4]={{},{"hour","minute","second"},{"day","month","year","week_day"},{"hour","minute","alarm_day"}};
	char *day_str[] = { "", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	char day_str_copy[5];
		

	uint8 i,flag=ON,pos[2],value_str[5];
	uint8 limits[4][4]={{},{24,59,59},{30,12,50,7},{24,59,7}};
	uint8 value=0;

	if ((options[option][position] == "hour")\
		|(options[option][position] == "day")){
			pos[0]=0;
			pos[1]=2;
	}
	else if ((options[option][position] == "minute")\
	|(options[option][position] == "month")){
			pos[0]=3;
			pos[1]=2;
	}
	else if ((options[option][position] == "second")){
			pos[0]=6;
			pos[1]=2;
	}
	else if ((options[option][position] == "year")){
			pos[0]=8;
			pos[1]=2;
	}
	else if ((options[option][position] == "week_day")){
			pos[0]=11;
			pos[1]=3;
	}
	else if ((options[option][position] == "alarm_day")){
			pos[0]=6;
			pos[1]=3;
	}
	if((options[option][position] == "week_day") | (options[option][position] == "alarm_day")){
		day_get(option_str_copy,' ',day_str_copy);
		value=day_search(day_str_copy);		
	}
	else if ((options[option][position] == "hour")\
	|(options[option][position] == "day")\ 
	|(options[option][position] == "minute")\
	|(options[option][position] == "month")\
	|(options[option][position] == "second")\
	|(options[option][position] == "year")){
		//LCD_displayStringRowColumn(0,0,options[option][position]);
		for(i=pos[0];i<pos[0]+pos[1];i++){
			value = (value*10) + (clock[option][i]-'0');
		}
	}
	while(flag){

		if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON3))
			(value==limits[option][position])? value==1:value++;

		else if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON2))
			(value==1)? value==limits[option][position]:value--;

		else if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON1)){
			strcpy(clock[option],option_str_copy);
			flag = OFF;
		}

		else if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON4))
			flag = OFF;
		if((options[option][position] == "week_day") | (options[option][position] == "alarm_day")){
			strcpy(value_str,day_str[value]);
		}
		else if ((options[option][position] == "hour")\
		|(options[option][position] == "day")\
		|(options[option][position] == "minute")\
		|(options[option][position] == "month")\
		|(options[option][position] == "second")\
		|(options[option][position] == "year")){
			sprintf(value_str,"%d",value);
			if(value<10){
				value_str[1]=value_str[0];
				value_str[0]='0';
				value_str[2]='\0';
			}
		}
		for(i=0;i<pos[1];i++){
			option_str_copy[pos[0]+i]=value_str[i];
		}
		LCD_displayStringRowColumn(2,0,option_str_copy);
	}
	switch(option){
		case 1:
			rtc_set_time(clock[TIME]);
			break;
		case 2:
			rtc_set_date(clock[DATE]);
			break;
		case 3:
			set_alarm(clock[ALARM]);
			break;		
	}
}

void select(uint8 option){
	char *menu[]= {"","Set Time","Set Date", "Set Alarm"};
	uint8 arrow_position=0,flag=ON,max=2;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,menu[option]);
	LCD_displayStringRowColumn(2,0,clock[option]);
	char *options[4][4]={{},{"hour","minute","second"},{"day","month","year","week_day"},{"hour","minute","alarm_day"}};
	if(2==option)
		max=3;
	while(flag){

		if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON3))
			(arrow_position==max)? arrow_position==0:arrow_position++;

		else if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON2))
			(arrow_position==0)? arrow_position==max:arrow_position--;

		else if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON1)){
			LCD_goToRowColumn(1,0);

			if ((options[option][arrow_position] == "hour")\
				|(options[option][arrow_position] == "day"))
				LCD_displayString("**             ");

			else if ((options[option][arrow_position] == "minute")\
					|(options[option][arrow_position] == "month"))
				LCD_displayString("   **          ");

			else if ((options[option][arrow_position] == "second"))
				LCD_displayString("      **       ");

			else if ((options[option][arrow_position] == "year"))
				LCD_displayString("      ****     ");

			else if ((options[option][arrow_position] == "week_day"))
				LCD_displayString("           *** ");
			else if ((options[option][arrow_position] == "alarm_day"))
				LCD_displayString("      ***      ");

			edit(option, arrow_position);
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,menu[option]);
			LCD_displayStringRowColumn(2,0,clock[option]);
		}

		else if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON4))
			flag = OFF;

		LCD_goToRowColumn(3,0);

		if ((options[option][arrow_position] == "hour")\
			|(options[option][arrow_position] == "day"))
			LCD_displayString("^^             ");

		else if ((options[option][arrow_position] == "minute")\
				|(options[option][arrow_position] == "month"))
			LCD_displayString("   ^^          ");

		else if ((options[option][arrow_position] == "second"))
			LCD_displayString("      ^^       ");

		else if ((options[option][arrow_position] == "year"))
			LCD_displayString("      ^^^^     ");

		else if ((options[option][arrow_position] == "week_day"))
			LCD_displayString("           ^^^ ");
		else if ((options[option][arrow_position] == "alarm_day"))
			LCD_displayString("      ^^^      ");

	}
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,1,"Digital Clock");
	LCD_displayStringRowColumn(1,1,"Set Time");
	LCD_displayStringRowColumn(2,1,"Set Date");
	LCD_displayStringRowColumn(3,1,"Set Alarm");
}

void options(){
	/* Set alarm
	 * Edit date
	 * Edit time
	 *
	 * */
	uint8 arrow_position=1, option_flag=ON, i;

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,1,"Digital Clock");
	LCD_displayStringRowColumn(1,1,"Set Time");
	LCD_displayStringRowColumn(2,1,"Set Date");
	LCD_displayStringRowColumn(3,1,"Set Alarm");

	while(option_flag){

		if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON3))
			(arrow_position==3)? arrow_position==1:arrow_position++;

		else if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON2))
			(arrow_position==1)? arrow_position==3:arrow_position--;

		else if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON1))
			select(arrow_position);

		else if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON4))
			option_flag = OFF;

		for(i=1;i<=3;i++){
			if(arrow_position==i){
				LCD_displayStringRowColumn(i,0,">");
				LCD_displayStringRowColumn(i,11,"<");
			}else{
				LCD_displayStringRowColumn(i,0," ");
				LCD_displayStringRowColumn(i,11," ");
			}
		}
	}

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,1,"Digital Clock");
	LCD_displayStringRowColumn(3,0,"Options");
	LCD_displayStringRowColumn(3,10,"Alarm");
}
