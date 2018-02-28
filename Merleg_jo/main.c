/*
 * Merleg.c
 *
 * Created: 2016.11.04. 18:44:42
 * Author : Dome
 */ 

#define F_CPU (16000000UL)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "LCD.h"
#include "button.h"
#include "LED.h"
#include "ADHX.h"
#include "USART_fifo.h"
#include "Fifo.h"
#include "HC05.h"
#include "scale_functions.h"
#include "LCD_string_functions.h"
#include "bluetooth_connection.h"

#define tomb_length	10
#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1

typedef struct Foods{
	uint8_t sorszam;		//a kaja sorszáma, lehet, hogy nem kell
	char nev[17];			//a kaja neve, sztringkent megadva
	float SZH_hund;			//szénhidrát 100 gramm-ban
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

Foods foods_tomb[tomb_length];						//egyes elemekre mutató tömb
uint8_t tomb_poz = 0;								//aktuális lista elem indexe
uint8_t tomb_poz_old = 0;							//eltárolásra, összehasonlításra szolgál
uint8_t menu_select_flag = 0;						//0 -> lista, 1 -> detailed

uint8_t communication_flag = 0;



float data_raw = 0;									//kiolvasott, formázatlan, nyers érték
float data_grams = 0;								//kiolvasott érték grammban
float data_SZH = 0;									//kiolvasott érték szénhidrátban
float old_data_raw = 0;								//összehasonlításhoz használt változó (lsd. lejjebb)
char data_grams_tomb[16];							//ftoa miatt tömb, grammoknak
char data_SZH_tomb[16];								//ftoa miatt tömb,SZH-nak

void timer1_init(void);								//0.1 sec-es timer
void peldanyosit(void);								//peldanyositja az egyes eteleket
void tombbe_tesz(void);								//beletesszük a tömbbe a példányosított struktúrákat
void menu1(Foods *);								//lista menü, kiválasztandó kajára mutató pointer
void menu2(Foods *);								//részletes (detailed) menü
void button(void);									//gombokat vizsgálja, hogy megnyomtuk e (részletes leírás a függvénynél)

void recieve_mode(void);

char string_rec[10];
char string_ready[10];

char buffer_main1[16] = {0};
char buffer_main2[16] = {0};
	
char string_tomb[16];

int main(void)
{
	timer1_init();							//timer1 inicializálása
	lcd_init();								//LCD inicializálása
	button_init();							//gombok inicializálása (led-nek is)
	LED_init();
	ADC_init();								//ADC inicializálása
	peldanyosit();							//kommentelve fent
	tombbe_tesz();							//kommentelve fent
	USART_init(MYUBRR);
	key_pin_init_pin_low();
	
	
	uint8_t tmp;
	
	while(1)									//KIRÁLY! billentyûrõl olvasó usart
	{
		if (tmp = (USART_data_receive()))
		{
			
			lcd_write_character(tmp);
			USART_data_transmit(tmp);
		}
	}
	
//	calibr = get_units(50);					//vesz egy értéket, amit majd késõbb kivon a "raw" (nem kalibrált) értékbõl
	
	//offset = calibr;
	
	while(1)
    {
		button();
		!menu_select_flag ? menu1(foods_tomb) : menu2(foods_tomb);	
	}
}

ISR(TIMER1_COMPA_vect)
{
	tick_timer1++;
	if (menu_select_flag)								//ha megnyomtuk a 3. gombot, csak akkor hajtja végre
	{
		//if (data_grams > 0)								//ha nagyobb a kiolvasott, kalibrált, gramm-ban értendõ érték, mint 0, csak akkor írja ki
		//{
			lcd_xy(0,0);								//kiírja a grammos értéket, a SZH értéket
			lcd_Puts("Grams: ");
			lcd_xy(7,0);
			lcd_Puts(data_grams_tomb);
			lcd_Puts(" g");
			lcd_xy(0,1);
			lcd_Puts("SZH: ");
			lcd_xy(7,1);
			lcd_Puts(data_SZH_tomb);
			lcd_Puts(" g");								//eddig
			
			
			/*
			if (communication_flag == 0)				//???????????????????????????????????????
			{
				USART_string_transmit("START");
				communication_flag = 2;
			}
			if (communication_flag == 2)
			{
				if (has_sentence())
				{
					USART_get_sentence(buffer_main1);
					if (!strcmp(buffer_main1,"READY\n"));
					{
						communication_flag = 1;
					}
				}
			}
			
			/*if (has_sentence())
			{
				USART_get_sentence(buffer_main2);
				if (!strcmp(buffer_main2,"START\n"))
				{
					USART_string_transmit("READY");
					communication_flag = 1;
				}
				lcd_xy(12,1);
				lcd_Puts(buffer_main2);
			}*/
		/*
			if ((has_sentence()) && (communication_flag == 1))
			{
				
				USART_get_sentence(buffer_main1);
				if (!strcmp(buffer_main1,"OK"))
				{
					USART_string_transmit(data_grams_tomb);
					USART_string_transmit("OK");
					buffer_main1[0] = '\0';
				}
				lcd_Puts("AAAAAAAA");
			}																				//?????????????????????
			
			tick_timer1 = 0;
		//}
		else
		{
			lcd_xy(0,0);								//ha negatív, akkor 0.00-t ír ki
			lcd_Puts("Grams: ");
			lcd_xy(7,0);
			lcd_Puts("0.00");
			lcd_Puts(" g");
			lcd_xy(0,1);
			lcd_Puts("SZH: ");
			lcd_xy(7,1);
			lcd_Puts("0.00");
			lcd_Puts(" g");								//eddig
		}*/
	}
}

void timer1_init()
{
	TCCR1B = (1 << WGM12) | (1 << CS12);				//CTC mód, 256-os elõosztás
	OCR1A = 6249;										//16000000/256/6250 = 10 Hz -> 1/10Hz = 10 ms
	TIMSK1 = (1 << OCIE1A);								//CTC mód engedélyez
	sei();												//interrupt engedélyez
}

void menu1(Foods *t)
{
	lcd_xy(0,0);									//kiválasztó > jel kitevése
	lcd_Puts(">");									//
	lcd_xy(1,0);									//kurzor -> 1. sor 2. hely (>-nek legyen helye)
	lcd_Puts(t[tomb_poz].nev);						//kaja kiíratása
	lcd_xy(1,1);									//pozícionálás
	if (tomb_poz != tomb_length-1)					//ha nem az utolsó elem,
	{
		lcd_Puts(t[(tomb_poz+1)].nev);				//akkor kiteszi az 1-el odébb lévõ kaját is
	}
	else											//ha az utolsó elem,
	{
		lcd_Puts(t[0].nev);							//akkor az 1.-t teszi ki
	}
}

void menu2(Foods *t)
{
	old_data_raw = data_raw;								//******, eltárolja az elõzõ értéket
	data_raw = get_units(10);								//beolvassa a raw értéket
	//data_grams = data_raw-calibr;							//levonja az elején beolvasott calibr-t, gram az eredmény
	//data_SZH = (data_grams * (t[tomb_poz].SZH_hund))/100;	//létrehozza a szénhidrát értéket, összeszorozza a gramot a 100 g-ban lévõ meghatározott szénhidrátokkal, majd elosztja 100-al
	//ftoa(data_grams,data_grams_tomb,2);						//átalakítja stringgé a grammokat, LCD-nek

	data_SZH = (data_raw * (t[tomb_poz].SZH_hund))/100;
	ftoa(data_raw,data_grams_tomb,2);	
	ftoa(data_SZH,data_SZH_tomb,2);							//átalakítja stringgé a szénhidrátot, LCD-nek
	/*
	if (data_raw < old_data_raw)							//****** összehasonlítja a régi beolvasott értéket, az újjal, különben összezavarodna a kiíratás, ha nagyobbról kicsire vált
	{
		lcd_xy(7,0);										//törli az értékeket (csak)
		lcd_Puts("         ");								//
		lcd_xy(7,0);										//egybõl kiírja az aktuális értéket
		lcd_Puts(data_grams_tomb);							//
		lcd_Puts(" g");										//
		lcd_xy(7,1);										// ---     ||     --- (u.a., mint fölül)
		lcd_Puts("         ");
		lcd_xy(7,1);
		lcd_Puts(data_SZH_tomb);
		lcd_Puts(" g");
	}
	*/
}

void button()
{
			tomb_poz_old = tomb_poz;					//	***  //
			
			if (btn1_pressed && (!menu_select_flag))	//1. gomb nyomása
			{
				tomb_poz++;								//tomb_poz-t lépteti, lépteti a kiírást
				if(tomb_poz == tomb_length)				//ha elérte az utolsó elemet -> visszalép az elsõre
				{
					tomb_poz = 0;						//
				}
				while(btn1_pressed);					//prellmentesít
			}
			if (btn2_pressed && (!menu_select_flag))	//2. gomb nyomása
			{
				if(tomb_poz == 0)						//ha elérte az elsõ elemet -> visszalép az utolsóra
				{
					tomb_poz = tomb_length;				//
				}
				tomb_poz--;								//tomb_poz-t lépteti, lépteti a kiírást
				while(btn2_pressed);					//prellmentesít
			}
			if (btn3_pressed)							//3. gomb nyomása
			{	
				menu_select_flag = (menu_select_flag == 0) ? 1 : 0;
				lcd_write_instruction(lcd_Clear);		//képernyõ törlése
				while(btn3_pressed);					//prellmentesít
			}
			if (tomb_poz_old != tomb_poz)				//csak akkor törli a képernyõt, ha változott a kiírt kaja ***
			{
				lcd_write_instruction(lcd_Clear);
			}
}

void peldanyosit()									//példányosítása a typedef-elt struktúrának
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

void tombbe_tesz()							//az egyes példányosított struktúrákat 1 tömbbe tevése (mutató hozzárendelése)
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