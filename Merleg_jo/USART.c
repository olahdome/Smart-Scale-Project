/*
 * USART.c
 *
 * Created: 2017.03.26. 18:46:48
 *  Author: Dome
 */ 

#include <avr/io.h>

void USART_init(uint16_t UBRR)
{
	UBRR0H = (unsigned char)(UBRR>>8);
	UBRR0L = (unsigned char)UBRR;
	
	/* Enable receiver and transmitter */
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	
	/* Set frame format: 8data, 2stop bit */
	UCSR0C |= (1<<USBS0)|(3<<UCSZ00);
}

void USART_data_transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while( !( UCSR0A & (1<<UDRE0)) );
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char USART_data_recieve(void)
{
	/* Wait for data to be received */
	while( !(UCSR0A & (1<<RXC0)) );
	
	/* Get and return received data from buffer */
	return UDR0;
}

void USART_string_transmit(char *string)
{
	while (*string)
	{
		USART_data_transmit(*string++);
	}
}

char * USART_string_recieve(char *recieve_string, char terminating_char)
{
	unsigned char temp = 0;
	uint8_t i = 0;
	
	while (1)
	{
		*(recieve_string + i) = USART_data_recieve();
	
		if(*(recieve_string + i)==terminating_char) break;
		else temp++;
		i++;
	}
	*(recieve_string + temp)='\0';
	return recieve_string;
}