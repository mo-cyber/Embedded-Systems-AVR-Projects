 /******************************************************************************
 *
 * Module: Ultrasonic
 *
 * File Name: ultrasonic.c
 *
 * Description: Source file for the Ultrasonic Sensor driver
 *
 * Author: Mohamed Ehab
 *
 *******************************************************************************/

#include "ultrasonic.h"
#include "icu.h"
#include "gpio.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

static uint8 g_edgeCount = 0;
static uint16 g_timeEcho = 0;

ISR(TIMER0_COMP_vect)
{
	GPIO_writePin(ULTRASONIC_TR_PORT_ID,ULTRASONIC_TR_PIN_ID,LOGIC_LOW);
	TCCR0 = 0;
}

static void Timer0_delay_ultrasonic_trigger(uint8 tick)
{
	TCNT0 = 0;

	OCR0  = tick;

	TIMSK |= (1<<OCIE0);

	TCCR0 = (1<<FOC0) | (1<<WGM01) | (1<<CS01);
}

void Ultrasonic_init(void)
{
	ICU_ConfigType ICU_Configurations = {F_CPU_8,RAISING};
	ICU_init(&ICU_Configurations);
	ICU_setCallBack(Ultrasonic_edgeProcessing);
	GPIO_setupPinDirection(ULTRASONIC_TR_PORT_ID,ULTRASONIC_TR_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(ULTRASONIC_ECHO_PORT_ID,ULTRASONIC_ECHO_PIN_ID,PIN_INPUT);
}

void Ultrasonic_Trigger(void)
{
	GPIO_writePin(ULTRASONIC_TR_PORT_ID,ULTRASONIC_TR_PIN_ID,LOGIC_HIGH);
	Timer0_delay_ultrasonic_trigger(10);
	//_delay_us(10);
	//GPIO_writePin(ULTRASONIC_TR_PORT_ID,ULTRASONIC_TR_PIN_ID,LOGIC_LOW);
}

uint16 Ultrasonic_readDistance(void)
{
	uint16 distanceValue;
	Ultrasonic_Trigger();
	while(g_edgeCount != 2);
	distanceValue = g_timeEcho / 58.8;
	g_edgeCount = 0;
	return distanceValue;

}

void Ultrasonic_edgeProcessing(void)
{
	if(g_edgeCount == 0)
	{
		g_edgeCount++;
		ICU_clearTimerValue();
		ICU_setEdgeDetectionType(FALLING);
	}
	else if(g_edgeCount == 1)
	{
		g_edgeCount++;
		g_timeEcho = ICU_getInputCaptureValue();
		ICU_setEdgeDetectionType(RAISING);
	}
}
