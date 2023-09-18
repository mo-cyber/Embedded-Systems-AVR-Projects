/*
 ================================================================================================
 Name        : distance_measurment.c
 Author      : Mohamed Ehab
 Description : Application to measure the distance using Ultrasonic Sensor, LCD, & ATmega32.
 Date        : 27/6/2023
 ================================================================================================
 */

#include "ultrasonic.h"
#include "lcd.h"
#include <avr/io.h>
#include <stdlib.h>

int main(void)
{
	uint16 distanceValue;
	SREG |= (1<<7);
	LCD_init();
	LCD_displayString("Distance =    cm");
	Ultrasonic_init();

	while(1)
	{
		distanceValue = Ultrasonic_readDistance();
		LCD_moveCursor(0,11);
		if(distanceValue >= 100)
		{
			LCD_intgerToString(distanceValue);
		}
		else
		{
			LCD_intgerToString(distanceValue);
			LCD_displayCharacter(' ');
		}
	}

	return 0;
}
