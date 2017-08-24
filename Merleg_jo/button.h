/*
 * button.h
 *
 * Created: 2016.11.13. 18:29:17
 *  Author: Dome
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_


#define btn1			PORTB2
#define btn2			PORTB3
#define btn3			PORTB4								//PORTC0
#define btn1_pressed	(PINB & (1 << PORTB2))
#define btn2_pressed	(PINB & (1 << PORTB3))
#define btn3_pressed	(PINB & (1 << PORTB4))

#define led				PORTB4
#define led_PORT		PORTB

extern void button_init(void);

#endif /* BUTTON_H_ */