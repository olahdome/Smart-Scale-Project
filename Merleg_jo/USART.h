/*
 * USART.h
 *
 * Created: 2017.03.26. 18:13:01
 *  Author: Dome
 */ 


#ifndef USART_H_
#define USART_H_

#include "inttypes.h"

void USART_init(uint16_t UBRR);
void USART_data_transmit(unsigned char data);
unsigned char USART_data_recieve(void);
void USART_string_transmit(char *string);
char * USART_string_recieve(char *recieve_string, char terminating_char);

#endif /* USART_H_ */