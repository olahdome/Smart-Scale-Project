/*
 * USART_fifo.c
 *
 * Created: 2017.10.26. 17:18:45
 *  Author: Dome
 */ 

#include "USART_fifo.h"
#include "Fifo.h"
#include <avr/interrupt.h>

#include "LCD.h"

static FIFO receive, transmit;
static uint8_t receive_buffer[receive_buffer_size], transmit_buffer[transmit_buffer_size];
volatile uint8_t sentence = 0;


void USART_init(uint16_t UBRR)
{
	UBRR0H = (unsigned char)(UBRR>>8);
	UBRR0L = (unsigned char)UBRR;
	
	/* Enable receiver and transmitter, and RX interrupt also */	
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	
	/* Set frame format: 8data, 2stop bit */
	//UCSR0C |= (1<<USBS0)|(3<<UCSZ00);

	/* Set frame format: 8data, 1stop bit */	
	UCSR0C |= (0<<USBS0)|(3<<UCSZ00);
	
	fifo_init(&receive, receive_buffer, receive_buffer_size);
	fifo_init(&transmit, transmit_buffer, transmit_buffer_size);
}

void USART_data_transmit(unsigned char data)
{
	fifo_put_in_data(&transmit, data);
	UCSR0B |= (1<<UDRIE0);
}

void USART_string_transmit(char *string)
{
	while (*string)
	{
		USART_data_transmit(*string);
		string++;
	}
}

/*
unsigned char USART_data_receive(void)
{
	return fifo_get_out_data(&receive);
}*/

uint8_t USART_data_receive(void)
{
	return fifo_get_out_data(&receive);
}

// char * USART_string_receive();	under construction


uint8_t has_sentence()
{
	lcd_xy(0,1);
	return sentence;
}

uint8_t USART_get_sentence(char *buffer)
{
	uint8_t tmp,i = 0;
	while((tmp = USART_data_receive()) != 12){ // 12 == '\n'
		lcd_xy(2,1);
		lcd_Puts("2");
		buffer[i] = tmp;
		i++;
	}
	buffer[i] = 0;
	sentence--;
	return i;
}

ISR(USART_RX_vect)
{
	/*uint8_t tmp = UDR0;
	if(tmp == 12) sentence++;
	fifo_put_in_data(&receive, tmp);
	UDR0 = tmp;*/
	
	fifo_put_in_data(&receive,UDR0);
	
}

ISR(USART_UDRE_vect)
{
	if (!transmit.fifo_elements)	// tilt
	{
		UCSR0B &= ~(1 << UDRIE0);
	}
	else
	{
		UDR0 = fifo_get_out_data(&transmit);
	}
}