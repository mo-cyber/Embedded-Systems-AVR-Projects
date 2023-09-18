/*
 ================================================================================================
 Name        : HMI_MC.c
 Author      : Mohamed Ehab
 Description : Code to control the door locker security system
 Date        : 8/8/2023
 ================================================================================================
 */

#include "uart.h"
#include "timer1.h"
#include "lcd.h"
#include "keypad.h"
#include <util/delay.h>
#include <avr/io.h>

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/*Definitions to ease the communication of the status and passCheck variables
 * between the two ECUs.*/
#define FAIL 0
#define PASS 1

#define ECU_READY 'x'

/*Global variable to count the times of timer1 interrupts to calculate the needed time.*/
uint8 g_tick = 0;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void enterPass(void);
uint8 mainOptions(void);
void checkPass(void);
void openDoor(void);
void lockSystem(void);


int main(void)
{
	/* - The two ecus communicate with 3 variables.
	 * - the first one is the status variable and this variable contain if the two
	 * passwords the user entered are matched (PASS) or not (FAIL) and I assumed that
	 * this variable is FAIL in the first loop to ask the user to enter the passwords
	 * at the beginning of the application.
	 * - the second variable is the option variable and it has the chosen main option
	 * by the user either to open the door or to change the password.
	 * - the third variable is the passCheck and this variable it to check if the
	 * entered password is right (PASS) or wrong (FAIL)*/
	uint8 status = FAIL, option, passCheck;
	/*Initialization of needed drivers as LCD, UART and Timer1*/
	LCD_init();
	UART_ConfigType UART_Configurations = {BIT8,DISABLED,BIT1,9600};
	UART_init(&UART_Configurations);
	Timer1_ConfigType Timer1_Configurations = {0,0,PRESCALING_1024,CTC};

	while(1)
	{
		/*The first condition is if the status FAIL it will ask the user to enter the
		 *  passwords but if it is PASS it will go to the main option.*/
		if(status == FAIL)
		{
			enterPass();
			status = UART_recieveByte();
		}
		else if(status == PASS)
		{
			option = mainOptions();
			UART_sendByte(option);
			uint8 counter = 4;
			/* - This loop is to ask the user for the passwords 3 times if the entered
			 * password is wrong but if the passCheck is PASS it will break
			 * the loop and starts to apply the chosen option.
			 * - After each loop the HMI ECU sends a ready byte to the Control ECU
			 * to keep both of them in the same loop.*/
			for(counter = 0; counter < 3; counter++)
			{
				checkPass();
				passCheck = UART_recieveByte();
				if(passCheck == PASS)
				{
					if(option == '-')
					{
						status = FAIL;
					}
					else if (option == '+')
					{
						Timer1_init(&Timer1_Configurations);
						Timer1_setCallBack(openDoor);
						/*This 1ms delay is to freeze the CPU until it access the
						 * call back function and increment the g_tick.*/
						_delay_ms(1);
						/*This while loop is to wait until call back function finish
						 * as it will put the g_tick = 0 again at the end of the function.*/
						while(g_tick != 0);
					}
					break;
				}
				UART_sendByte(ECU_READY);
			}
			/*If counter = 3 means that the previous for loop finished all
			 * without passing the passCheck so here it will call the lockSystem call
			 * back function.*/
			if(counter == 3)
			{
				Timer1_init(&Timer1_Configurations);
				Timer1_setCallBack(lockSystem);
				_delay_ms(1);
				while(g_tick != 0);
			}
		}
	}

	return 0;
}

/*Description:
 * - The function is responsible to take the two passwords from the user and send them
 * to the control ECU.
 * - The function is also responsible to interface with the user through the LCD and the
 * keypad to take the needed passwords. */
void enterPass(void)
{
	uint8 pass1[5] ,pass2[5];
	uint8 counter1 = 0, counter2 = 0, pressedKey;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"plz enter pass:");
	LCD_moveCursor(1,0);
	/* - The function here takes 5 digits of the pass1 and these digits must be
	 * from 0 to 9 and it will not accept more or less than 5 digits.
	 * - It will display '*' every time the user enters a digit.*/
	while(counter1 < 5)
	{
		pressedKey = KEYPAD_getPressedKey();
		_delay_ms(500);

		if(pressedKey>='0' && pressedKey<='9')
		{
			pass1[counter1] = pressedKey;
			LCD_displayCharacter('*');
			counter1++;
		}
		else
		{
			//do nothing
		}
	}

	/* -The while(1) here is to force the program not to go to the next step until
	 * the user press '=' then it will ask for pass2 and it take it in the same way
	 * as described in pass1.*/
	while(1)
	{
		if(KEYPAD_getPressedKey() == '=')
		{
			_delay_ms(500);
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"plz re-enter the");
			LCD_displayStringRowColumn(1,0,"same pass:");
			LCD_moveCursor(1,11);
			while(counter2 < 5)
			{
				pressedKey = KEYPAD_getPressedKey();
				_delay_ms(500);

				if(pressedKey>='0' && pressedKey<='9')
				{
					pass2[counter2] = pressedKey;
					LCD_displayCharacter('*');
					counter2++;
				}
				else
				{
					//do nothing
				}
			}
			break;
		}
		else
		{
			//do nothing
		}
	}
	/*Here it will start sending the two passwords to the Control ECU if the user
	 * enter '=' for the second time.*/
	while(1)
	{
		if(KEYPAD_getPressedKey() == '=')
		{
			_delay_ms(500);
			for(uint8 i = 0; i<5; i++)
			{
				UART_sendByte(pass1[i]);
			}
			for(uint8 i = 0; i<5; i++)
			{
				UART_sendByte(pass2[i]);
			}
			break;
		}
		else
		{
			//do nothing
		}
	}

}

/*Description:
 * - This function is the one that will always appears to the user in the home screen
 * after finishing any task.
 * - The function is only responsible to ask the user if he wants to open door or change
 * the password.*/
uint8 mainOptions(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"+ : Open Door");
	LCD_displayStringRowColumn(1,0,"- : Change Pass");
	return (KEYPAD_getPressedKey());
}

/*Description:
 *The function is responsible to ask the user for the password to apply his choice
 *and send this password to the Control ECU. */
void checkPass(void)
{
	uint8 pass[5];
	uint8 pressedKey, counter = 0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"plz enter pass: ");
	LCD_moveCursor(1,0);
	while(counter < 5)
	{
		pressedKey = KEYPAD_getPressedKey();
		_delay_ms(500);

		if(pressedKey>='0' && pressedKey<='9')
		{
			pass[counter] = pressedKey;
			LCD_displayCharacter('*');
			counter++;
		}
		else
		{
			//do nothing
		}
	}
	for(uint8 i = 0; i<5; i++)
	{
		UART_sendByte(pass[i]);
	}
}

/*Description:
 * - This is a call back function and it will be called if the user choose to
 * open the door.
 * - It is responsible to display if the door is locking or not on the LCD and to
 * calculate the needed time by changing the number in the OCR1A register every g_tick.*/
void openDoor(void)
{
	/* - Prescaler = 1024
	 * - CPU frequency = 8 Mhz
	 * - timer frequency = 8*10^6 / 1024 = 7812.5 hz
	 * - clock cycle = 1 / 7812.5 = 1.28*10^-4 sec
	 * - 2^16 = 65536 (0 -> 65535)
	 * - 18 sec (Door is Unlocking) -> 18 / 1.28*10^-4 = 140625 clock cycle (65535 + 65535 + 9555)
	 * - 15 sec (Door is Locking) -> 15 / 1.28*10^-4 = 117187 clock cycle (65535 + 51652)*/
	g_tick++;
	if (g_tick == 1)
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Door is");
		LCD_displayStringRowColumn(1,0,"Unlocking");
		OCR1A = 65535;
	}
	else if (g_tick == 2)
	{
		OCR1A = 65535;
	}
	else if (g_tick == 3)
	{
		OCR1A = 9555;
	}
	else if (g_tick == 4)
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Door is Locking");
		OCR1A = 65535;
	}
	else if (g_tick == 5)
	{
		OCR1A = 51652;
	}
	else if (g_tick == 6)
	{
		LCD_clearScreen();
		g_tick = 0;
		Timer1_deInit();
	}
}

/*Description:
 * - This is a call back function and it will be called if the user enters the
 * password wrong 3 times.
 * - It is responsible to display an error message on the LCD for 1 minute.*/
void lockSystem(void)
{
	/*prescaler = 1024
	 * CPU frequency = 8 Mhz
	 * timer frequency = 8*10^6 / 1024 = 7812.5 hz
	 * clock cycle = 1 / 7812.5 = 1.28*10^-4 sec
	 * 2^16 = 65536 (0 -> 65535)
	 * 1 min (60 sec) -> 60 / 1.28*10^-4 = 468750 clock cycle ((7 * 65535) + 10005)*/
	g_tick++;
	if(g_tick == 1)
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Emsek 7aramyyyy");
		OCR1A = 65535;
	}
	else if (g_tick > 1 && g_tick < 8)
	{
		OCR1A = 65535;
	}
	else if (g_tick == 8)
	{
		OCR1A = 10005;
	}
	else if (g_tick == 9)
	{
		LCD_clearScreen();
		g_tick = 0;
		Timer1_deInit();
	}
}
