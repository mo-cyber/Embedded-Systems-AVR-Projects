/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: header file for the ATmega32 buzzer driver
 *
 * Author: Mohamed Ehab
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define BUZZER_PORT_ID PORTC_ID
#define BUZZER_PIN_ID PIN2_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*Description:
 * the function setup the direction for the buzzer pin as output pin through the GPIO
 * driver and it turn off the buzzer through the GPIO. */
void Buzzer_init();

/*Description:
 * the function to enable the Buzzer through the GPIO. */
void Buzzer_on(void);

/*Description:
 * the function to disable the Buzzer through the GPIO.*/
void Buzzer_off(void);

#endif /* BUZZER_H_ */
