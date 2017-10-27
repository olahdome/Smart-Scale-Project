/*
 * Fifo.h
 *
 * Created: 2017.10.26. 17:19:49
 *  Author: Dome
 */ 


#ifndef FIFO_H_
#define FIFO_H_

#include "inttypes.h"

typedef struct{

	uint8_t* data;
	uint8_t fifo_size;
	uint8_t fifo_write_pointer;
	uint8_t fifo_read_pointer;
	uint8_t fifo_elements;

}FIFO;

void fifo_init(FIFO* fifo, uint8_t* data, uint8_t size);
uint8_t fifo_get_out_data(FIFO* fifo);
uint8_t fifo_put_in_data(FIFO* fifo, uint8_t data);

#endif /* FIFO_H_ */