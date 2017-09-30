/*
 * ADHX.c
 *
 * Created: 2016.11.17. 18:10:16
 *  Author: Dome
 */ 
#ifndef F_CPU
#define F_CPU (16000000UL)
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "ADHX.h"
#include "button.h"

extern void ADC_init()
{
	DDRD |= (1 << PD_SCK);
	DDRD &= ~(1 << DOUT);
}

extern uint32_t readAD(void)
{
	uint8_t i;
	uint32_t count = 0;
		
	PORTD |= (1 << DOUT);					//DOUT = 1
	PORTD &= ~(1 << PD_SCK);				//PD_SCK = 0;
	
	while(PIND & (1 << DOUT));
	
	for (i = 0; i < 24; i++)
	{
		PORTD |= (1 << PD_SCK);
		_delay_us(1);
		PORTD &= ~(1 << PD_SCK);
		_delay_us(1);
		count = count << 1;
		if (PIND & (1 << DOUT)) count++;
	}
	
	PORTD |= (1 << PD_SCK);
	_delay_us(1);
	PORTD &= ~(1 << PD_SCK);
	count = count ^ 0x800000;
	
	return count;
}

