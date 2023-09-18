/*
 ================================================================================================
 Name        : fan_controller.c
 Author      : Mohamed Ehab
 Description : Application for fan controller based on LM35, LCD, and DC Motor.
 Date        : 27/6/2023
 ================================================================================================
 */

#include "adc.h"
#include "dc_motor.h"
#include "lcd.h"

int main(void)
{
	ADC_ConfigType ADC_Configurations = {INTERNAL_2_56V,F_ADC_8};
	ADC_init(&ADC_Configurations);
	DcMotor_Init();
	LCD_init();
	LCD_displayStringRowColumn(0,0,"FAN is ");
	LCD_displayStringRowColumn(1,0,"Temp =     C");
	while(1)
	{
		uint8 tempValue = LM35_getTemperature();
		if(tempValue < 30)
		{
			DcMotor_Rotate(STOP,0);
			LCD_displayStringRowColumn(0,7,"OFF");

		}
		else if (tempValue >= 30 && tempValue < 60)
		{
			DcMotor_Rotate(CW,25);
			LCD_displayStringRowColumn(0,7,"ON ");

		}
		else if (tempValue >= 60 && tempValue < 90)
		{
			DcMotor_Rotate(CW,50);
			LCD_displayStringRowColumn(0,7,"ON ");
		}
		else if (tempValue >= 90 && tempValue < 120)
		{
			DcMotor_Rotate(CW,75);
			LCD_displayStringRowColumn(0,7,"ON ");
		}
		else if (tempValue >= 120)
		{
			DcMotor_Rotate(CW,100);
			LCD_displayStringRowColumn(0,7,"ON ");
		}
		LCD_moveCursor(1,7);
		if(tempValue >= 100)
		{
			LCD_intgerToString(tempValue);
		}
		else
		{
			LCD_intgerToString(tempValue);
			LCD_displayCharacter(' ');
		}
	}

	return 0;
}

