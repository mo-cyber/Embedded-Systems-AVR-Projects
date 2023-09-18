/******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.c
 *
 * Description: Source file for the ATmega32 DC Motor driver
 *
 * Author: Mohamed Ehab
 *
 *******************************************************************************/
#include "dc_motor.h"
#include "gpio.h"
#include "pwm_timer0.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void DcMotor_Init(void)
{
	GPIO_setupPinDirection(L293D_IN1_PORT_ID,L293D_IN1_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(L293D_IN2_PORT_ID,L293D_IN2_PIN_ID,PIN_OUTPUT);
	GPIO_writePin(L293D_IN1_PORT_ID,L293D_IN1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(L293D_IN2_PORT_ID,L293D_IN2_PIN_ID,LOGIC_LOW);
#if(NUM_OF_MOTORS > 1)
	GPIO_setupPinDirection(L293D_IN3_PORT_ID,L293D_IN3_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(L293D_IN4_PORT_ID,L293D_IN4_PIN_ID,PIN_OUTPUT);
	GPIO_writePin(L293D_IN3_PORT_ID,L293D_IN3_PIN_ID,LOGIC_LOW);
	GPIO_writePin(L293D_IN4_PORT_ID,L293D_IN4_PIN_ID,LOGIC_LOW);
#endif
}

void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	if(state == CW)
	{
		GPIO_writePin(L293D_IN1_PORT_ID,L293D_IN1_PIN_ID,LOGIC_HIGH);
		GPIO_writePin(L293D_IN2_PORT_ID,L293D_IN2_PIN_ID,LOGIC_LOW);
#if(NUM_OF_MOTORS > 1)
		GPIO_writePin(L293D_IN3_PORT_ID,L293D_IN3_PIN_ID,LOGIC_HIGH);
		GPIO_writePin(L293D_IN4_PORT_ID,L293D_IN4_PIN_ID,LOGIC_LOW);
#endif
	}
	else if(state == A_CW)
	{
		GPIO_writePin(L293D_IN1_PORT_ID,L293D_IN1_PIN_ID,LOGIC_LOW);
		GPIO_writePin(L293D_IN2_PORT_ID,L293D_IN2_PIN_ID,LOGIC_HIGH);
#if(NUM_OF_MOTORS > 1)
		GPIO_writePin(L293D_IN3_PORT_ID,L293D_IN3_PIN_ID,LOGIC_LOW);
		GPIO_writePin(L293D_IN4_PORT_ID,L293D_IN4_PIN_ID,LOGIC_HIGH);
#endif
	}
	else if(state == STOP)
	{
		GPIO_writePin(L293D_IN1_PORT_ID,L293D_IN1_PIN_ID,LOGIC_LOW);
		GPIO_writePin(L293D_IN2_PORT_ID,L293D_IN2_PIN_ID,LOGIC_LOW);
#if(NUM_OF_MOTORS > 1)
		GPIO_writePin(L293D_IN3_PORT_ID,L293D_IN3_PIN_ID,LOGIC_LOW);
		GPIO_writePin(L293D_IN4_PORT_ID,L293D_IN4_PIN_ID,LOGIC_LOW);
#endif
	}
	PWM_Timer0_Start(speed);
}
