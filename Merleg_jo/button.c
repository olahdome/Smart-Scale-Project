/*
 * button.c
 *
 * Created: 2016.11.13. 18:33:51
 *  Author: Dome
 */ 

//button.c
#ifndef F_CPU
#define F_CPU (16000000UL)
#endif

#include <avr/io.h>
#include "button.h"

extern void button_init()
{
	DDRB &= ~((1 << btn1) | (1 << btn2) | (1 << btn3));
}