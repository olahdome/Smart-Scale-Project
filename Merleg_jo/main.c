/*
 * Merleg.c
 *
 * Created: 2016.11.04. 18:44:42
 * Author : Dome
 */ 

#define F_CPU (16000000UL)

#include <avr/io.h>
//#include <avr/iom128.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "LCD.h"
#include "button.h"
#include "ADHX.h"
#include "USART.h"
#include "HC05.h"

#define tomb_length		10

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

typedef struct Foods{
	uint8_t sorszam;		//a kaja sorsz�ma, lehet, hogy nem kell
	char nev[17];			//a kaja neve, sztringkent megadva
	float SZH_hund;			//sz�nhidr�t 100 gramm-ban
}Foods;

Foods alma;			//1.
Foods banan;		//2.
Foods korte;		//3.
Foods narancs;		//4.
Foods szolo;		//5.
Foods krumpli;		//6.
Foods liszt;		//7.
Foods rizs;			//8.
Foods spagetti;		//9.
Foods tarhonya;		//10.

uint16_t tick_timer1 = 0;

Foods foods_tomb[tomb_length];						//egyes elemekre mutat� t�mb
uint8_t tomb_poz = 0;								//aktu�lis lista elem indexe
uint8_t tomb_poz_old = 0;							//elt�rol�sra, �sszehasonl�t�sra szolg�l
uint8_t menu_select_flag = 0;						//0 -> lista, 1 -> detailed
//uint8_t menu1_flag = 1, menu2_flag = 0;				

//uint32_t offset = 6400;								//offset be�ll�t�sa
uint32_t offset = 0;
//uint32_t offset = 8594600;								
//float scale = 417.6;								//kalibr�ci� be�ll�t�sa
float scale = 401.3;
uint32_t calibr = 0;								//kalibr�ci� be�ll�t�sa, elej�n beolvas, ezzel kappunk grammokat

float data_raw = 0;									//kiolvasott, form�zatlan, nyers �rt�k
float data_grams = 0;								//kiolvasott �rt�k grammban
float data_SZH = 0;									//kiolvasott �rt�k sz�nhidr�tban
float old_data_raw = 0;								//�sszehasonl�t�shoz haszn�lt v�ltoz� (lsd. lejjebb)
char data_grams_tomb[16];							//ftoa miatt t�mb, grammoknak
char data_SZH_tomb[16];								//ftoa miatt t�mb,SZH-nak


void timer1_init(void);								//0.1 sec-es timer
void peldanyosit(void);								//peldanyositja az egyes eteleket
void tombbe_tesz(void);								//beletessz�k a t�mbbe a p�ld�nyos�tott strukt�r�kat
void menu1(Foods *);								//lista men�, kiv�lasztand� kaj�ra mutat� pointer
void menu2(Foods *);								//r�szletes (detailed) men�
void button(void);									//gombokat vizsg�lja, hogy megnyomtuk e (r�szletes le�r�s a f�ggv�nyn�l)

void ftoa(float n, char *res, int afterpoint);		//float to string
int intToStr(int x, char str[], int d);				//int to string
void reverse(char *str, int len);					//megford�tja a stringet
uint32_t read_average(uint8_t);						//beolvas annyiszor, amennyi sz�mot adunk param�ternek, kisz�molja az �tlag�t
double get_value(uint8_t times);					//az �tlagb�l levonja az offsetet
float get_units(uint8_t times);						//get_value-t (�tlagb�l levont offsetet) m�g elosztja a scale �rt�kkel (kalibr�l� �rt�k)
void tare(uint8_t);									//t�r�z


char string_rec[10];
char string_ready[10];

int main(void)
{
	timer1_init();							//timer1 inicializ�l�sa
	lcd_init();								//LCD inicializ�l�sa
	button_init();							//gombok inicializ�l�sa (led-nek is)
	ADC_init();								//ADC inicializ�l�sa
	peldanyosit();							//kommentelve fent
	tombbe_tesz();							//kommentelve fent
	
	USART_init(MYUBRR);
	
	calibr = get_units(50);					//vesz egy �rt�ket, amit majd k�s�bb kivon a "raw" (nem kalibr�lt) �rt�kb�l
	
	while(1)
    {
	
		button();
		
		if (!menu_select_flag)									//ha 3. gomb nem volt m�g megnyomva
		{
			menu1(foods_tomb);									//men� megh�v�sa, lista men�
		}
		if (menu_select_flag)									//ha 3. gombot megnyomtuk
		{
			menu2(foods_tomb);									//menu megh�v�sa, r�szletes (detailed) men�
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	tick_timer1++;
	//uint8_t i = 0;
	
	if (menu_select_flag)								//ha megnyomtuk a 3. gombot, csak akkor hajtja v�gre
	{
		if (data_grams > 0)								//ha nagyobb a kiolvasott, kalibr�lt, gramm-ban �rtend� �rt�k, mint 0, csak akkor �rja ki
		{
			lcd_xy(0,0);								//ki�rja a grammos �rt�ket, a SZH �rt�ket
			lcd_Puts("Grams: ");
			lcd_xy(7,0);
			lcd_Puts(data_grams_tomb);
			lcd_Puts(" g");
			lcd_xy(0,1);
			lcd_Puts("SZH: ");
			lcd_xy(7,1);
			lcd_Puts(data_SZH_tomb);
			lcd_Puts(" g");								//eddig
			
			//if (tick_timer1 == 50)
			//{
			//hc_05_bluetooth_transmit_string("ready");
			//if ((strcmp(string_ready,"")))
			//{
				//if  (!(strcmp((hc_05_bluetooth_receive_string(string_ready, 'y')),"ready")))
				//{
			//if (tick_timer1 == 50)
			//{	
				hc_05_bluetooth_transmit_string(data_grams_tomb);
				hc_05_bluetooth_transmit_string("OK");
				tick_timer1 = 0;
			//}
				//}
			//}
		/*		if (!(strcmp(string_ready,"ready")))
				{
					while (*string_ready)
					{						
						string_ready[i] = 0;
						i++;
					}
				}*/
				
			//	hc_05_bluetooth_transmit_string("trim");
			//	tick_timer1 = 0;
			//}
			
		}
		else
		{
			lcd_xy(0,0);								//ha negat�v, akkor 0.00-t �r ki
			lcd_Puts("Grams: ");
			lcd_xy(7,0);
			lcd_Puts("0.00");
			lcd_Puts(" g");
			lcd_xy(0,1);
			lcd_Puts("SZH: ");
			lcd_xy(7,1);
			lcd_Puts("0.00");
			lcd_Puts(" g");								//eddig
		}
	}
}

void timer1_init()
{
	TCCR1B = (1 << WGM12) | (1 << CS12);				//CTC m�d, 256-os el�oszt�s
	OCR1A = 6249;										//16000000/256/6250 = 10 Hz -> 1/10Hz = 10 ms
	TIMSK1 = (1 << OCIE1A);								//CTC m�d enged�lyez
	sei();												//interrupt enged�lyez
}



void menu1(Foods *t)
{
	lcd_xy(0,0);									//kiv�laszt� > jel kitev�se
	lcd_Puts(">");									//
	lcd_xy(1,0);									//kurzor -> 1. sor 2. hely (>-nek legyen helye)
	lcd_Puts(t[tomb_poz].nev);						//kaja ki�rat�sa
	lcd_xy(1,1);									//poz�cion�l�s
	if (tomb_poz != 9)								//ha nem az utols� elem,
	{
		lcd_Puts(t[(tomb_poz+1)].nev);				//akkor kiteszi az 1-el od�bb l�v� kaj�t is
	}
	if (tomb_poz == tomb_length-1)					//ha az utols� elem,
	{
		lcd_Puts(t[0].nev);							//akkor az 1.-t teszi ki
	}
}

void menu2(Foods *t)
{
	old_data_raw = data_raw;								//******, elt�rolja az el�z� �rt�ket
	data_raw = get_units(10);								//beolvassa a raw �rt�ket
	data_grams = data_raw-calibr;							//levonja az elej�n beolvasott calibr-t, gram az eredm�ny
	data_SZH = (data_grams * (t[tomb_poz].SZH_hund))/100;	//l�trehozza a sz�nhidr�t �rt�ket, �sszeszorozza a gramot a 100 g-ban l�v� meghat�rozott sz�nhidr�tokkal, majd elosztja 100-al
	ftoa(data_grams,data_grams_tomb,2);						//�talak�tja stringg� a grammokat, LCD-nek
	ftoa(data_SZH,data_SZH_tomb,2);							//�talak�tja stringg� a sz�nhidr�tot, LCD-nek
	if (data_raw < old_data_raw)							//****** �sszehasonl�tja a r�gi beolvasott �rt�ket, az �jjal, k�l�nben �sszezavarodna a ki�rat�s, ha nagyobbr�l kicsire v�lt
	{
		lcd_xy(7,0);										//t�rli az �rt�keket (csak)
		lcd_Puts("         ");								//
		lcd_xy(7,0);										//egyb�l ki�rja az aktu�lis �rt�ket
		lcd_Puts(data_grams_tomb);							//
		lcd_Puts(" g");										//
		lcd_xy(7,1);										// ---     ||     --- (u.a., mint f�l�l)
		lcd_Puts("         ");
		lcd_xy(7,1);
		lcd_Puts(data_SZH_tomb);
		lcd_Puts(" g");
	}
}

void button()
{
			tomb_poz_old = tomb_poz;					//	***
			//
			if (btn1_pressed && (!menu_select_flag))	//1. gomb nyom�sa
			{
				tomb_poz++;								//tomb_poz-t l�pteti, l�pteti a ki�r�st
				if(tomb_poz == tomb_length)				//ha el�rte az utols� elemet -> visszal�p az els�re
				{
					tomb_poz = 0;						//
				}
				while(btn1_pressed);					//prellmentes�t
			}
			if (btn2_pressed && (!menu_select_flag))	//2. gomb nyom�sa
			{
				if(tomb_poz == 0)						//ha el�rte az els� elemet -> visszal�p az utols�ra
				{
					tomb_poz = tomb_length;				//
				}
				tomb_poz--;								//tomb_poz-t l�pteti, l�pteti a ki�r�st
				while(btn2_pressed);					//prellmentes�t
			}
			if (btn2_pressed && (menu_select_flag))		//	pr�ba, nem j�!
			{
				tare(50);
				while (btn2_pressed);
			}
			if (btn3_pressed)							//3. gomb nyom�sa
			{
				menu_select_flag = ~menu_select_flag;	//menu v�laszt� neg�l�sa
			
				lcd_write_instruction(lcd_Clear);		//k�perny� t�rl�se
				while(btn3_pressed);					//prellmentes�t
			}
			
			if (tomb_poz_old != tomb_poz)				//csak akkor t�rli a k�perny�t, ha v�ltozott a ki�rt kaja ***
			{
				lcd_write_instruction(lcd_Clear);
			}
}

void peldanyosit()									//p�ld�nyos�t�sa a typedef-elt strukt�r�nak
{	
	alma.sorszam		= 1;
	strcpy(alma.nev, "Alma");
	alma.SZH_hund		= 11.41;
	
	banan.sorszam		= 2;
	strcpy(banan.nev, "Banan");
	banan.SZH_hund		= 22.8;
	
	korte.sorszam		= 3;
	strcpy(korte.nev, "Korte");
	korte.SZH_hund		= 15.4;
	
	narancs.sorszam		= 4;
	strcpy(narancs.nev, "Narancs");
	narancs.SZH_hund	= 8.5;
	
	szolo.sorszam		= 5;
	strcpy(szolo.nev, "Szolo");
	szolo.SZH_hund		= 18.1;
	
	krumpli.sorszam		= 6;
	strcpy(krumpli.nev, "Fott krumpli");
	krumpli.SZH_hund	= 18.5;
	
	liszt.sorszam		= 7;
	strcpy(liszt.nev, "Liszt");
	liszt.SZH_hund		= 75;
	
	rizs.sorszam		= 8;
	strcpy(rizs.nev, "Rizs");
	rizs.SZH_hund		= 28.2;
	
	spagetti.sorszam	= 9;
	strcpy(spagetti.nev, "Spagetti");
	spagetti.SZH_hund	= 30.9;
	
	tarhonya.sorszam	= 10;
	strcpy(tarhonya.nev, "Tarhonya");
	tarhonya.SZH_hund	= 27;
}

void tombbe_tesz()							//az egyes p�ld�nyos�tott strukt�r�kat 1 t�mbbe tev�se (mutat� hozz�rendel�se)
{
	foods_tomb[0] = alma;
	foods_tomb[1] = banan;
	foods_tomb[2] = korte;
	foods_tomb[3] = narancs;
	foods_tomb[4] = szolo;
	foods_tomb[5] = krumpli;
	foods_tomb[6] = liszt;
	foods_tomb[7] = rizs;
	foods_tomb[8] = spagetti;
	foods_tomb[9] = tarhonya;
}

//-----------------------------------------------//

void ftoa(float n, char *res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;

	// Extract floating part
	float fpart = n - (float)ipart;

	// convert integer part to string
	int i = intToStr(ipart, res, 1);

	// check for display option after point
	if (afterpoint != 0)
	{
		res[i] = '.';  // add dot

		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);

		intToStr((int)fpart, res + i + 1, afterpoint);
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

extern uint32_t read_average(uint8_t times)
{
	uint32_t sum = 0;
	uint8_t i = 0;
	
	while(i < times)
	{
		sum += readAD();
		i++;
	}
	
	return sum/times;
}

extern double get_value(uint8_t times)
{
	return read_average(times) - offset;
}

extern float get_units(uint8_t times)
{
	return get_value(times) / scale;
}

extern void tare(uint8_t times)
{
	uint32_t sum = get_units(times);
	calibr = sum;
/*	lcd_xy(7,0);										//t�rli az �rt�keket (csak)
	lcd_Puts("         ");								//
	lcd_xy(7,1);										// ---     ||     --- (u.a., mint f�l�l)
	lcd_Puts("         ");
*/
}

//-----------------------------------------------------------------------------------------------------//

		/*fasza, ne t�r�ld ki! (t�r�lhet� m�r)
		old_data = data;
		data = get_units(10);
		data = data-calibr;
		ftoa(data,tomb,2);
		if (data < old_data)
		{
			//lcd_write_instruction(lcd_Clear);
			lcd_xy(0,1);
			lcd_Puts("                ");
		}*/
		