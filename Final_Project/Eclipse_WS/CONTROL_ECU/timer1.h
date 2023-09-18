/******************************************************************************
 *
 * Module: Timer 1
 *
 * File Name: timer1.h
 *
 * Description: header file for the ATmega32 timer 1 driver
 *
 * Author: Mohamed Ehab
 *
 *******************************************************************************/

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

typedef enum
{
	NO_CLOCK,NO_PRESCALING,PRESCALING_8,PRESCALING_64,PRESCALING_256,PRESCALING_1024,EXTERNAL_FALLING,EXTERNAL_RISING
}Timer1_Prescaler;

typedef enum
{
	NORMAL,CTC
}Timer1_Mode;

typedef struct {
     uint16 initial_value;
     uint16 compare_value; // it will be used in compare mode only.
     Timer1_Prescaler prescaler;
     Timer1_Mode mode;
} Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*Description:
 * Function to initialize the Timer driver. */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/*Description:
 * Function to disable the Timer1.*/
void Timer1_deInit(void);

/*Description:
 * Function to set the Call Back function address.*/
void Timer1_setCallBack(void(*a_ptr)(void));

#endif /* TIMER1_H_ */
