/*
 * LCD_string_functions.c
 *
 * Created: 2017.10.02. 23:14:38
 *  Author: Dome
 */ 

#ifndef F_CPU
#define F_CPU (16000000UL)
#endif

#include <string.h>
#include <math.h>
#include "inttypes.h"
#include "LCD_string_functions.h"

void ftoa(float n, char *res, int afterpoint)
{
	uint8_t negative_flag = 0;
	
	if (n < 0)
	{
		n *= (-1);
		res[0] = '-';
		negative_flag = 1;
	}
	
	// Extract integer part
	int ipart = (int)n;

	// Extract floating part
	float fpart = n - (float)ipart;
	
	int i;

	// convert integer part to string
	if (negative_flag == 0)
	{
		i = intToStr(ipart, res, 1);
	}
	else
	{
		i = intToStr(ipart, res + 1, 1);
	}
	// check for display option after point
	if (afterpoint != 0)
	{
		if (negative_flag == 0)
		{
			res[i] = '.';  // add dot
		}
		else
		{
			res[i+1] = '.';  // add dot
		}

		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);

		if (negative_flag == 0)
		{
			intToStr((int)fpart, res + i + 1, afterpoint);
		}
		else
		{
			intToStr((int)fpart, res + i + 2, afterpoint);
		}
		

	}
}
int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x)
	{
		str[i++] = (x%10) + '0';
		x = x/10;
	}
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
	str[i++] = '0';

	reverse(str, i);
	str[i] = '\0';
	return i;
}

void reverse(char *str, int len)
{
	int i=0, j=len-1, temp;
	while (i<j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}