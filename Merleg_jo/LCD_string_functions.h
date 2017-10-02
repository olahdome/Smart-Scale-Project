/*
 * LCD_string_functions.h
 *
 * Created: 2017.10.02. 23:14:56
 *  Author: Dome
 */ 

#ifndef LCD_STRING_FUNCTIONS_H_
#define LCD_STRING_FUNCTIONS_H_

extern void ftoa(float n, char *res, int afterpoint);		//float to string
extern int intToStr(int x, char str[], int d);				//int to string
extern void reverse(char *str, int len);					//megfordítja a stringet

#endif /* LCD_STRING_FUNCTIONS_H_ */