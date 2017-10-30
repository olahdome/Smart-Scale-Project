/*
 * scale_functions.c
 *
 * Created: 2017.09.30. 23:26:37
 *  Author: Dome
 */

#ifndef F_CPU
#define F_CPU (16000000UL)
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "scale_functions.h"
#include "ADHX.h"

//uint32_t calibr = 0;
double offset = 21446.4;		//8593500 ??
float scale = 401;			//401.3; <--- jó érték

extern uint32_t read_average(uint8_t times)
{
	uint32_t sum = 0;
	uint8_t i = 0;
	
	while(i < times)
	{
		sum += readAD();
		i++;
	}
	return sum/times;
}
/*
extern double get_value(uint8_t times)
{
	return read_average(times) - offset;
}

extern float get_units(uint8_t times)
{
	return get_value(times) / scale;
}
*/
extern double get_value(uint8_t times)
{
	return read_average(times) / scale;
}

extern float get_units(uint8_t times)
{
	return get_value(times) - offset;
}

extern void tare(uint8_t times)
{
	double sum = get_units(times);
	//calibr = sum;
}

extern void set_offset(double new_offset)
{
	offset = new_offset;
}

extern void set_scale(float new_scale)
{
	scale = new_scale;
}