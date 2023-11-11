/******************************************************************************
 *
 * Module: Timer 1
 *
 * File Name: timer1.c
 *
 * Description: Source file for the ATmega32 timer 1 driver
 *
 * Author: Mohamed Ehab
 *
 *******************************************************************************/

#include "timer1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the compare match */
		(*g_callBackPtr)();
	}
}

ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the counter overflow */
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	/*TCCR1A Register:
	 * COM1A1 = 0, COM1A0 = 0, COM1B1 = 0, COM1B0 = 0 (Normal port operation, OC1A/OC1B disconnected.)
	 * FOC1A = 1, FOC1B = 1 (specifies a non-PWM mode)
	 * WGM10 = 0, WGM11 = 0 (either in normal or compare mode )*/
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);

	/*TCCR1B Register:
	 * WGM12 = 0 (Normal mode), WGM12 = 1 (CTC Mode)
	 * WGM13 = 0 (either in normal or compare mode)
	 * ICNC1 = 0 (deactivates the Input Capture Noise Canceler)
	 * ICES1 = 0 (not used)
	 * CS10, CS11, CS12 (based on timer1 prescaler configuration)*/

	/*insert the required clock value in the first three bits (CS10, CS11 and CS12)*/
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);

	/*insert the required mode in WGM12 bit in TCCR1B Register*/
	TCCR1B = (TCCR1B & 0xF7) | ((Config_Ptr->mode)<<3);

	/* Initial Value for Timer1 */
	TCNT1 = Config_Ptr->initial_value;

	/* Compare Value for Timer1 in CTC mode*/
	if(Config_Ptr->mode == CTC)
	{
		/* Compare Value for Timer1 in CTC mode*/
		OCR1A = Config_Ptr->compare_value;
		/* Enable the Output Compare A Match Interrupt */
		TIMSK |= (1<<OCIE1A);
	}
	else if(Config_Ptr->mode == NORMAL)
	{
		/* Enable the Overflow Interrupt */
		TIMSK |= (1<<TOIE1);
	}

	/*Enable the I-bit*/
	SREG |= (1<<7);
}

void Timer1_deInit(void)
{
	/* Clear All Timer1/ICU Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;

	/* Disable the output compare A match and overflow interrupt */
	TIMSK &= ~(1<<OCIE1A) & ~(1<<TOIE1);

	/* Reset the global pointer value */
	g_callBackPtr = NULL_PTR;

	/* Disable the i-bit */
	SREG &= ~(1<<7);
}

void Timer1_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}



