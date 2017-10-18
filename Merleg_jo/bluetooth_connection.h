/*
 * bluetooth_connection.h
 *
 * Created: 2017.10.17. 21:11:39
 *  Author: Dome
 */

#ifndef BLUETOOTH_CONNECTION_H_
#define BLUETOOTH_CONNECTION_H_

#define key_port	PORTB
#define key_bit		PORTB5
#define key_ddr		DDRB

extern void key_pin_init();
extern void enter_AT_mode();
extern void AT_mode_say(char *AT_command);

#endif /* BLUETOOTH_CONNECTION_H_ */