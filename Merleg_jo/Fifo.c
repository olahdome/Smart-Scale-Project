/*
 * Fifo.c
 *
 * Created: 2017.10.26. 17:19:13
 *  Author: Dome
 */ 

#include "Fifo.h"
#include "inttypes.h"

void fifo_init(FIFO* fifo, uint8_t* data, uint8_t size)
{
	fifo->data = data;
	fifo->fifo_read_pointer = 0;
	fifo->fifo_write_pointer = 0;
	fifo->fifo_size = size-1;
	fifo->fifo_elements = 0;	
}

uint8_t fifo_get_out_data(FIFO* fifo)
{
	if(!fifo->fifo_elements) return 0;

	uint8_t uiReturn = fifo->data[fifo->fifo_read_pointer];
	
	fifo->fifo_read_pointer++;
	fifo->fifo_read_pointer %= fifo->fifo_size;
	fifo->fifo_elements--;
	
	return uiReturn;
}

uint8_t fifo_put_in_data(FIFO* fifo, uint8_t data)
{
	if((fifo->fifo_size) == (fifo->fifo_elements)) return 0;
	
	(fifo->data[fifo->fifo_write_pointer]) = data;
	
	fifo->fifo_write_pointer++;
	fifo->fifo_elements++;
	
	return 1;
}