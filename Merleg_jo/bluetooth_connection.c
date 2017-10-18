/*
 * bluetooth_connection.c
 *
 * Created: 2017.10.17. 21:11:24
 *  Author: Dome
 */

#include <avr/io.h>
#include <inttypes.h>
#include "bluetooth_connection.h"
#include "HC05.h"

void key_pin_init()
{
	key_ddr |= (1 << key_bit);
	key_port &= ~(1 << key_bit);
}

void enter_AT_mode()
{
	key_port |= (1 << key_bit);
}

void AT_mode_say(char *AT_command)
{
	enter_AT_mode();
	
	hc_05_bluetooth_transmit_string(AT_command);
}