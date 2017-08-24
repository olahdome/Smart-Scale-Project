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

/*
void readAD(uint8_t *p1, uint8_t *p2, uint8_t *p3)
{
	uint8_t i;
	uint8_t d1=0, d2=0, d3=0;						//data1, data2, data3
	
	PORTD |= (1 << DOUT);					//DOUT = 1
	PORTD &= ~(1 << PD_SCK);				//PD_SCK = 0;
	
	
	while(!(PIND & (1 << DOUT)));
	
	for (i = 0; i < 24; i++)
	{
		PORTD |= (1 << PD_SCK);				//PD_SCK = 1;
		
		if(i<8)
		{
			d1 = d1 << 1;
			if(PIND & (1 << DOUT)) d1++;
		}
		if((i>= 8) && (i<16))
		{
			d2 = d2 << 1;
			if(PIND & (1 << DOUT)) d2++;
		}
		if(i>=16)
		{
			d3 = d3 << 1;
			if(PIND & (1 << DOUT)) d3++;
		}

		
		PORTD &= ~(1 << PD_SCK);			//PD_SCK = 0;
		
	}
	PORTD |= (1 << PD_SCK);					//PD_SCK = 1;
	d3 = d3 ^ 0x80;
	PORTD &= ~(1 << PD_SCK);				//PD_SCK = 0;
	
	*p1 = d1;
	*p2 = d2;
	*p3 = d3;
}
*/

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

