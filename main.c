/************************************************
 *	 File Name: main.c							*
 * Description: 								*
 *  Created on: Oct 25, 2018					*
 *      Author: Khalid Tarek					*
 ************************************************/
#include"Digital_Clock.h"

int main(void)
{
	clock_init();

	while (1)
	{
		if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON1))
			options();

		if(button_is_pressed(&BUTTON_CTRL_PORT_IN, BUTTON4))
			alarm();

		LCD_display_update();
		if(check_alarm()==1){
			SET_BIT(BUZZER_CTRL_PORT_OUT,BUZZER);
		}else{
			CLEAR_BIT(BUZZER_CTRL_PORT_OUT,BUZZER);//toggle led every 0.5 second
		}
		
	}

	return 0;
}
