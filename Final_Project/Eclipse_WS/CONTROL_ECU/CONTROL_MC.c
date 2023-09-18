/*
 ================================================================================================
 Name        : CONTROL_MC.c
 Author      : Mohamed Ehab
 Description : Code to interface with the door locker security system
 Date        : 8/8/2023
 ================================================================================================
 */

#include "twi.h"
#include "uart.h"
#include "timer1.h"
#include "buzzer.h"
#include "external_eeprom.h"
#include "dc_motor.h"
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

/*The address which the password will be saved in the external EEPROM.*/
#define EEPROM_ADDRESS 0x0311

/*Global variable to count the times of timer1 interrupts to calculate the needed time.*/
uint8 g_tick = 0;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

uint8 receive2pass(void);
uint8 checkPass(void);
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
	/*Initialization of needed drivers as UART, TWI, DC motor, Buzzer, and Timer1*/
	UART_ConfigType UART_Configurations = {BIT8,DISABLED,BIT1,9600};
	UART_init(&UART_Configurations);
	TWI_ConfigType TWI_Configurations = {1,FAST_MODE};
	TWI_init(&TWI_Configurations);
	DcMotor_Init();
	Buzzer_init();
	Timer1_ConfigType Timer1_Configurations = {0,0,PRESCALING_1024,CTC};
	while(1)
	{
		/*The first condition is if the status FAIL it will receive the 2 passwords
		 * and send the new status to the HMI ECU but if it is PASS it will
		 * receive the chosen option and start handle it.*/
		if (status == FAIL)
		{
			status = receive2pass();
			UART_sendByte(status);
		}
		else if(status == PASS)
		{
			option = UART_recieveByte();
			uint8 counter = 0;
			/* - This loop is to check the password every time the HMI ECU send it.
			 * - The password will be checked maximum 3 times if the entered
			 * password is wrong but if the passCheck is PASS it will break
			 * the loop and starts to apply the chosen option.
			 * - After each loop the Control ECU receive a ready byte to start the
			 * next loop and to keep both of them in the same loop.*/
			while(counter < 3)
			{
				passCheck = checkPass();
				UART_sendByte(passCheck);
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
				/*This loop is to wait until the HMI ECU finish its loop and send the
				 * ready byte.*/
				while(UART_recieveByte() != ECU_READY);
				counter++;
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
 * - The function will receive the 2 passwords from the HMI ECU.
 * - It will return FAIL if the 2 passwords are unmatched and it will save the password
 * in the external EEPROM if the 2 passwords are matched and return PASS.*/
uint8 receive2pass(void)
{
	uint8 pass1[5],pass2[5];
	for(uint8 i = 0; i<5; i++)
	{
		pass1[i] = UART_recieveByte();
	}
	for(uint8 i = 0; i<5; i++)
	{
		pass2[i] = UART_recieveByte();
		if(pass2[i] != pass1[i])
		{
			return FAIL;
		}
	}
	for(uint8 i = 0; i<5; i++)
	{
		EEPROM_writeByte(EEPROM_ADDRESS + i, pass1[i]);
		_delay_ms(10);
	}

	return PASS;
}

/*Description:
 * - The function receive the entered password from the HMI ECU and compare it with
 * the password in the external EEPROM.
 * - The function return PASS if the received password match the stored one
 * and return FAIL if they are not. */
uint8 checkPass(void)
{
	uint8 pass[5];
	uint8 val;
	for(uint8 i = 0; i<5; i++)
	{
		pass[i] = UART_recieveByte();
		EEPROM_readByte(EEPROM_ADDRESS + i,&val);
		if (pass[i] != val)
		{
			return FAIL;
		}
	}
	return PASS;
}

/*Description:
 * - This is a call back function and it will be called if the user choose to
 * open the door.
 * - It is responsible to rotate the motor CW, A_CW, STOP on its maximum speed and to
 * calculate the needed time by changing the number in the OCR1A register every g_tick.*/
void openDoor(void)
{
	/* - Prescaler = 1024
		 * - CPU frequency = 8 Mhz
		 * - timer frequency = 8*10^6 / 1024 = 7812.5 hz
		 * - clock cycle = 1 / 7812.5 = 1.28*10^-4 sec
		 * - 2^16 = 65536 (0 -> 65535)
		 * - 15 sec (CW & A_CW) -> 15 / 1.28*10^-4 = 117187 clock cycle (65535 + 51652)
		 * - 3 sec (STOP) -> 3 / 1.28*10^-4 = 23437 clock cycle*/
	g_tick++;
	if (g_tick == 1)
	{
		DcMotor_Rotate(CW,100);
		OCR1A = 65535;
	}
	else if (g_tick == 2)
	{
		OCR1A = 51652;
	}
	else if (g_tick == 3)
	{
		DcMotor_Rotate(STOP,0);
		OCR1A = 23437;
	}
	else if (g_tick == 4)
	{
		DcMotor_Rotate(A_CW,100);
		OCR1A = 65535;
	}
	else if (g_tick == 5)
	{
		OCR1A = 51652;
	}
	else if (g_tick == 6)
	{
		DcMotor_Rotate(STOP,0);
		g_tick = 0;
		Timer1_deInit();
	}
}

/*Description:
 * - This is a call back function and it will be called if the user enters the
 * password wrong 3 times.
 * - It is responsible to turn on the buzzer for 1 minute then turn it off.*/
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
		Buzzer_on();
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
		Buzzer_off();
		g_tick = 0;
		Timer1_deInit();
	}
}

