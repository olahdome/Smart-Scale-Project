//LCD.c
#ifndef F_CPU
#define F_CPU (16000000UL)
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"

extern void lcd_init(void)
{
	lcd_D7_ddr |= (1<<lcd_D7_bit);                  // 4 data lines - output
	lcd_D6_ddr |= (1<<lcd_D6_bit);
	lcd_D5_ddr |= (1<<lcd_D5_bit);
	lcd_D4_ddr |= (1<<lcd_D4_bit);
	
	lcd_E_ddr |= (1<<lcd_E_bit);                    // E line - output
	lcd_RS_ddr |= (1<<lcd_RS_bit);                  // RS line - output

	// Power-up delay
	_delay_ms(100);                                 // initial 40 mSec delay

	// IMPORTANT - At this point the LCD module is in the 8-bit mode and it is expecting to receive
	//   8 bits of data, one bit on each of its 8 data lines, each time the 'E' line is pulsed.
	//
	// Since the LCD module is wired for the 4-bit mode, only the upper four data lines are connected to
	//   the microprocessor and the lower four data lines are typically left open.  Therefore, when
	//   the 'E' line is pulsed, the LCD controller will read whatever data has been set up on the upper
	//   four data lines and the lower four data lines will be high (due to internal pull-up circuitry).
	//
	// Fortunately the 'FunctionReset' instruction does not care about what is on the lower four bits so
	//   this instruction can be sent on just the four available data lines and it will be interpreted
	//   properly by the LCD controller.  The 'lcd_write_4' subroutine will accomplish this if the
	//   control lines have previously been configured properly.

	// Set up the RS and E lines for the 'lcd_write_4' subroutine.
	lcd_RS_port &= ~(1<<lcd_RS_bit);                // select the Instruction Register (RS low)
	lcd_E_port &= ~(1<<lcd_E_bit);                  // make sure E is initially low

	// Reset the LCD controller
	lcd_write(lcd_FunctionReset);                 // first part of reset sequence
	_delay_ms(10);                                  // 4.1 mS delay (min)

	lcd_write(lcd_FunctionReset);                 // second part of reset sequence
	_delay_us(200);                                 // 100uS delay (min)

	lcd_write(lcd_FunctionReset);                 // third part of reset sequence
	_delay_us(200);                                 // this delay is omitted in the data sheet

	// Preliminary Function Set instruction - used only to set the 4-bit mode.
	// The number of lines or the font cannot be set at this time since the controller is still in the
	//  8-bit mode, but the data transfer mode can be changed since this parameter is determined by one
	//  of the upper four bits of the instruction.
	
	lcd_write(lcd_FunctionSet4bit);               // set 4-bit mode
	_delay_us(80);                                  // 40uS delay (min)

	// Function Set instruction
	lcd_write_instruction(lcd_FunctionSet4bit);   // set mode, lines, and font
	_delay_us(80);                                  // 40uS delay (min)

	// The next three instructions are specified in the data sheet as part of the initialization routine,
	//  so it is a good idea (but probably not necessary) to do them just as specified and then redo them
	//  later if the application requires a different configuration.

	// Display On/Off Control instruction
	lcd_write_instruction(lcd_DisplayOff);        // turn display OFF
	_delay_us(80);                                  // 40uS delay (min)

	// Clear Display instruction
	lcd_write_instruction(lcd_Clear);             // clear display RAM
	_delay_ms(4);                                   // 1.64 mS delay (min)

	// ; Entry Mode Set instruction
	lcd_write_instruction(lcd_EntryMode);         // set desired shift characteristics
	_delay_us(80);                                  // 40uS delay (min)

	// This is the end of the LCD controller initialization as specified in the data sheet, but the display
	//  has been left in the OFF condition.  This is a good time to turn the display back ON.
	
	// Display On/Off Control instruction
	lcd_write_instruction(lcd_DisplayOn);         // turn the display ON
	//_delay_us(80);                                  // 40uS delay (min)
	
	//lcd_write_instruction(0x08 | (1<<0x2) | (1<<1) | (0<<0));      //LCD be, alahuzas, kurzor villog
	_delay_us(80);
}

extern void lcd_write(uint8_t theByte)
{
	lcd_D7_port &= ~(1<<lcd_D7_bit);                        // assume that data is '0'
	if (theByte & 1<<7) lcd_D7_port |= (1<<lcd_D7_bit);     // make data = '1' if necessary

	lcd_D6_port &= ~(1<<lcd_D6_bit);                        // repeat for each data bit
	if (theByte & 1<<6) lcd_D6_port |= (1<<lcd_D6_bit);

	lcd_D5_port &= ~(1<<lcd_D5_bit);
	if (theByte & 1<<5) lcd_D5_port |= (1<<lcd_D5_bit);

	lcd_D4_port &= ~(1<<lcd_D4_bit);
	if (theByte & 1<<4) lcd_D4_port |= (1<<lcd_D4_bit);

	// write the data
	// 'Address set-up time' (40 nS)
	lcd_E_port |= (1<<lcd_E_bit);                   // Enable pin high
	_delay_us(1);                                   // implement 'Data set-up time' (80 nS) and 'Enable pulse width' (230 nS)
	lcd_E_port &= ~(1<<lcd_E_bit);                  // Enable pin low
	_delay_us(1);                                   // implement 'Data hold time' (10 nS) and 'Enable cycle time' (500 nS)
}

extern void lcd_write_instruction(uint8_t theInstruction)
{
	lcd_RS_port &= ~(1<<lcd_RS_bit);                // select the Instruction Register (RS low)
	lcd_E_port &= ~(1<<lcd_E_bit);                  // make sure E is initially low
	lcd_write(theInstruction);                    // write the upper 4-bits of the data
	lcd_write(theInstruction << 4);               // write the lower 4-bits of the data
}

extern void lcd_write_character(uint8_t theData)
{
	lcd_RS_port |= (1<<lcd_RS_bit);                 // select the Data Register (RS high)
	lcd_E_port &= ~(1<<lcd_E_bit);                  // make sure E is initially low
	lcd_write(theData);                           // write the upper 4-bits of the data
	lcd_write(theData << 4);                      // write the lower 4-bits of the data
}

extern void lcd_Puts(char theString[])
{
	//volatile int i = 0;                             // character counter*/
	while (*theString)
	{
		lcd_write_character(*theString);
		theString++;
		_delay_us(80);                              // 40 uS delay (min)
	}
}

void lcd_xy(unsigned char x, unsigned char y){
	switch(y){
		case 0: {y=0x00 + 0x80 + x; break;}
		case 1: {y=0x40 + 0x80 + x; break;}
		default: break;
	}
	lcd_write_instruction((uint8_t)y);
	lcd_Puts(" ");
}