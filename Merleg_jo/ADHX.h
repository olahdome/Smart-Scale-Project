/*
 * ADHX.h
 *
 * Created: 2016.11.17. 18:10:31
 *  Author: Dome
 */ 

//MIN: 800000 -> MAX: 7FFFFF


#ifndef ADHX_H_
#define ADHX_H_

#define PD_SCK		PORTD2
#define DOUT		PORTD3

extern void ADC_init(void);
extern uint32_t readAD(void);

#endif /* ADHX_H_ */