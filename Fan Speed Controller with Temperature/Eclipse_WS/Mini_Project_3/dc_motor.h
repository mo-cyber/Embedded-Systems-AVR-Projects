/******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.h
 *
 * Description: header file for the ATmega32 DC Motor driver
 *
 * Author: Mohamed Ehab
 *
 *******************************************************************************/


#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum
{
	CW,A_CW,STOP
}DcMotor_State;

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define NUM_OF_MOTORS 1

#define L293D_IN1_PORT_ID PORTB_ID
#define L293D_IN1_PIN_ID PIN0_ID

#define L293D_IN2_PORT_ID PORTB_ID
#define L293D_IN2_PIN_ID PIN1_ID

#if(NUM_OF_MOTORS > 1)

#define L293D_IN3_PORT_ID PORTB_ID
#define L293D_IN3_PIN_ID PIN0_ID

#define L293D_IN4_PORT_ID PORTB_ID
#define L293D_IN4_PIN_ID PIN0_ID

#endif

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*Description:
 * The Function responsible for setup the direction for the two motor pins through
 * the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver. */
void DcMotor_Init(void);

/*Description:
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor
 * 	based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the required speed value. */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DC_MOTOR_H_ */
