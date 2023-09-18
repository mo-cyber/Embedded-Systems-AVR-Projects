/*
 * Mini_Project_2.c
 * Stop Watch System
 * Created on: May 22, 2023
 * Author: Mohamed Ehab
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//The alarm definition is to decide if you want this option or not.
#define ALARM 1

//variable for each segment in the stopwatch.
unsigned char SEC1 = 0, SEC2 = 0, MIN1 = 0, MIN2 = 0, HOUR1 = 0, HOUR2 = 0;
//variable for each segment in the alarm.
//the counter variable is to decide which segment you want to set for the alarm.
unsigned char SEC3 = 0, SEC4 = 0, MIN3 = 0, MIN4 = 0, HOUR3 = 0, HOUR4 = 0, counter = 0;

//the isr of interrupt0 will reset the timer counter and the variables of each segment in the stopwatch.
ISR(INT0_vect)
{
	TCNT1 = 0;
	SEC1 = 0, SEC2 = 0, MIN1 = 0, MIN2 = 0, HOUR1 = 0, HOUR2 = 0;
}

//the isr of interrupt1 will stop the timer
ISR(INT1_vect)
{
	TCCR1B &= ~(1<<CS10) & ~(1<<CS11);
}

//the isr of interrupt2 will wnable the timer again.
ISR(INT2_vect)
{
	TCCR1B |= (1<<CS10) | (1<<CS11);
}

//the isr of timer1 interrupt will deal with the seconds, minutes, and hours for the stopwatch.
ISR(TIMER1_COMPA_vect)
{
	if(SEC1 < 9)
	{
		SEC1++;
	}
	else
	{
		if(SEC2 < 5)
		{
			SEC1 = 0;
			SEC2++;
		}
		else
		{
			if(MIN1 < 9)
			{
				SEC1 = 0, SEC2 = 0;
				MIN1++;
			}
			else
			{
				if(MIN2 < 5)
				{
					SEC1 = 0, SEC2 = 0, MIN1 = 0;
					MIN2++;
				}
				else
				{
					if(HOUR1 < 9)
					{
						SEC1 = 0, SEC2 = 0, MIN1 = 0, MIN2 = 0;
						HOUR1++;
					}
					else
					{
						if(HOUR2 < 9)
						{
							SEC1 = 0, SEC2 = 0, MIN1 = 0, MIN2 = 0, HOUR1 = 0;
							HOUR2++;
						}
						else
						{
							SEC1 = 0, SEC2 = 0, MIN1 = 0, MIN2 = 0, HOUR1 = 0, HOUR2 = 0;
						}
					}
				}
			}
		}

	}
}

//the function will enable the needed pin, interrupt on falling edge, and enable the module interrupt
void Interrupt0_Enable(void)
{
	DDRD &= ~(1<<PD2);
	PORTD |= (1<<PD2);
	MCUCR |= (1<<ISC01);
	MCUCR &= ~(1<<ISC00);
	GICR |= (1<<INT0);
}

//the function will enable the needed pin, interrupt on rising edge, and enable the module interrupt
void Interrupt1_Enable(void)
{
	DDRD &= ~(1<<PD3);
	MCUCR |= (1<<ISC10) | (1<<ISC11);
	GICR |= (1<<INT1);
}

//the function will enable the needed pin, interrupt on falling edge, and enable the module interrupt
void Interrupt2_Enable(void)
{
	DDRB &= ~(1<<PB2);
	PORTB |= (1<<PB2);
	MCUCSR &= ~(1<<ISC2);
	GICR |= (1<<INT2);
}

/*the function will reset the timer counter, reset the output compare to interrupt
every 1 second, enable the module interrupt, and choose 64 prescalar.*/
void Timer1_CTC_Enable(void)
{
	TCNT1 = 0;
	OCR1A = 15625;
	TIMSK = (1<<OCIE1A);
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS11);
}

//the function is to enable the stopwatch 7-segments pins
void Segments_Enable(void)
{
	DDRA |= 0x3F;
	PORTA &= 0xC0;
	DDRC |= 0x0F;
	PORTC &= 0xF0;
}

//the function is to enable the alarm 7-segments pins
/*to deal with the alarm, we have 2 push buttons the first 2 is to choose which
 * segment you want to deal with and the second one is to increment the segment to set
 * it on the needed time.*/
void Alarm_Enable(void)
{
	DDRB &= ~(1<<PB0) & ~(1<<PB1);
	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7);
	DDRA |= (1<<PA6);
	PORTA &= ~(1<<PA6);
	DDRC |= 0xF0;
	PORTC &= 0x0F;
	DDRB |= 0xF8;
	PORTB &= 0x07;
}

//this function is to deal with the alarm settings.
void Alarm_Count(void)
{
	/*here is the first button and it is increment the counter variable
	 * to decide which segment you want to set(hours, minutes, seconds)*/
	if(PINB & (1<<PB1))
	{
		_delay_ms(30);
		if(PINB & (1<<PB1))
		{
			PORTA |= (1<<PA7);
			if(counter == 5)
			{
				counter = 0;
			}
			else
			{
				counter++;
			}
		}while(PINB & (1<<PB1));
	}
	/*here is the second button and it is increment the time you want to set*/
	if(PINB & (1<<PB0))
	{
		_delay_ms(30);
		if(PINB & (1<<PB0))
		{
			PORTA |= (1<<PA7);
			if(counter == 0)
			{
				if(SEC3 == 9)
				{
					SEC3 = 0;
				}
				else
				{
					SEC3++;
				}
			}
			else if(counter == 1)
			{
				if(SEC4 == 6)
				{
					SEC4 = 0;
				}
				else
				{
					SEC4++;
				}
			}
			else if(counter == 2)
			{
				if(MIN3 == 9)
				{
					MIN3 = 0;
				}
				else
				{
					MIN3++;
				}
			}
			else if(counter == 3)
			{
				if(MIN4 == 6)
				{
					MIN4 = 0;
				}
				else
				{
					MIN4++;
				}
			}
			else if(counter == 4)
			{
				if(HOUR3 == 9)
				{
					HOUR3 = 0;
				}
				else
				{
					HOUR3++;
				}
			}
			else if(counter == 5)
			{
				if(HOUR4 == 9)
				{
					HOUR4 = 0;
				}
				else
				{
					HOUR4++;
				}
			}
		}while(PINB & (1<<PB0));
	}
}

int main(void)
{
	SREG |= (1<<7);
	Interrupt0_Enable();
	Interrupt1_Enable();
	Interrupt2_Enable();
	Segments_Enable();
	Timer1_CTC_Enable();
	Alarm_Enable();
	while(1)
	{
		/*every loop i check the alarm count function and check if the stopwatch equals
		 * to the alarm and if they are equal the buzzer will be enabled.
		 * this option will work only if the alarm definition is 1.
		 * then i enable each segment in the 12 segments and display the needed variable on it.*/
		Alarm_Count();
		if((SEC1 == SEC3) && (SEC2 == SEC4) && (MIN1 == MIN3) && (MIN2 == MIN4) && (HOUR1 == HOUR3) && (HOUR2 == HOUR4) && ALARM)
		{
			PORTA &= ~(1<<PA7);
		}
		PORTA = (PORTA & 0xC0) | (0x01);
		PORTC = (PORTC & 0xF0) | (SEC1 & 0x0F);
		PORTA &= ~(1<<PA6);
		PORTB = (PORTB & 0x07) | (0x08);
		PORTC = (PORTC & 0x0F) | ((SEC3<<4) & 0xF0);
		_delay_ms(2);
		PORTA = (PORTA & 0xC0) | (0x02);
		PORTC = (PORTC & 0xF0) | (SEC2 & 0x0F);
		PORTA &= ~(1<<PA6);
		PORTB = (PORTB & 0x07) | (0x10);
		PORTC = (PORTC & 0x0F) | ((SEC4<<4) & 0xF0);
		_delay_ms(2);
		PORTA = (PORTA & 0xC0) | (0x04);
		PORTC = (PORTC & 0xF0) | (MIN1 & 0x0F);
		PORTA &= ~(1<<PA6);
		PORTB = (PORTB & 0x07) | (0x20);
		PORTC = (PORTC & 0x0F) | ((MIN3<<4) & 0xF0);
		_delay_ms(2);
		PORTA = (PORTA & 0xC0) | (0x08);
		PORTC = (PORTC & 0xF0) | (MIN2 & 0x0F);
		PORTA &= ~(1<<PA6);
		PORTB = (PORTB & 0x07) | (0x40);
		PORTC = (PORTC & 0x0F) | ((MIN4<<4) & 0xF0);
		_delay_ms(2);
		PORTA = (PORTA & 0xC0) | (0x10);
		PORTC = (PORTC & 0xF0) | (HOUR1 & 0x0F);
		PORTA &= ~(1<<PA6);
		PORTB = (PORTB & 0x07) | (0x80);
		PORTC = (PORTC & 0x0F) | ((HOUR3<<4) & 0xF0);
		_delay_ms(2);
		PORTA = (PORTA & 0xC0) | (0x20);
		PORTC = (PORTC & 0xF0) | (HOUR2 & 0x0F);
		PORTB = (PORTB & 0x07);
		PORTA |= (1<<PA6);
		PORTC = (PORTC & 0x0F) | ((HOUR4<<4) & 0xF0);
		_delay_ms(2);
	}
}

