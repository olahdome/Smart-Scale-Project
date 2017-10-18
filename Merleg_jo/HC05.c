/*
 * HC05.c
 *
 * Created: 2017.03.26. 20:42:36
 *  Author: Dome
 */ 

#ifndef F_CPU
#define F_CPU (16000000UL)
#endif

#include "HC05.h"
#include "USART.h"
#include "inttypes.h"

void hc_05_bluetooth_transmit_byte(char data_byte)
{
	USART_data_transmit(data_byte);
}

char hc_05_bluetooth_receive_byte(void)
{
	return USART_data_recieve();
}

void hc_05_bluetooth_transmit_string(char *transmit_string)
{
	USART_string_transmit(transmit_string);
}

char *hc_05_bluetooth_receive_string(char *receive_string, char terminating_character)
{
	unsigned char temp = 0;
	uint8_t i = 0;
	while (1)
	{
		*(receive_string+i)=USART_data_recieve();
		
		if(*(receive_string+i)==terminating_character) break;
		else temp++;
		i++;
	}
	*(receive_string+temp)='\0';
	return receive_string;
}