/*
 * bluetooth_connection.c
 *
 * Created: 2017.10.17. 21:11:24
 *  Author: Dome
 */
/* <------------ ezt
#include <avr/io.h>
#include <inttypes.h>
#include "bluetooth_connection.h"
#include "HC05.h"
#include "LCD.h"
#include "button.h"

#define bluetooth_array_row		3
#define bluetooth_array_column	17

uint8_t actual_command = 0;
char bluetooth_options[bluetooth_array_row][bluetooth_array_column]	=	{
																			"AT",
																			"AT+INIT",
																			"AT+INQ"
																		};

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

void bluetooth_menu()
{
	while (1)
	{
		command_picker();
		lcd_xy(0,0);
		lcd_Puts(">");
		lcd_xy(1,0);
		lcd_Puts(bluetooth_options[actual_command]);
		lcd_xy(1,1);
		if (actual_command != (bluetooth_array_row - 1))
		{
			lcd_Puts(bluetooth_options[actual_command + 1]);
		}
		else
		{
			lcd_Puts(bluetooth_options[0]);
		}
		
		if (btn3_pressed)
		{
			lcd_write_instruction(lcd_Clear);
			return;
			while(btn3_pressed);
		}
	}
}

void command_picker()
{
	if (btn1_pressed)
	{
		actual_command = (actual_command + 1) % 3;
		lcd_write_instruction(lcd_Clear);
		while(btn1_pressed);
	}
	if (btn2_pressed)
	{
		if (actual_command == 0)
		{
			actual_command = 3;
		}
		actual_command = (actual_command - 1);
		lcd_write_instruction(lcd_Clear);
		while(btn2_pressed);
	}
}*/