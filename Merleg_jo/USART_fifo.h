/*
 * USART_fifo.h
 *
 * Created: 2017.10.26. 17:19:00
 *  Author: Dome
 */ 


#ifndef USART_FIFO_H_
#define USART_FIFO_H_

#define receive_buffer_size		128
#define transmit_buffer_size	128 // or 16???

#include <inttypes.h>

void USART_init(uint16_t UBRR);
void USART_data_transmit(unsigned char data);
void USART_string_transmit(char *string);
unsigned char USART_data_receive(void);
uint8_t has_sentence(void);
uint8_t USART_get_sentence(char *buffer);
uint8_t USART_string_receive(char *buffer);

#endif /* USART_FIFO_H_ */