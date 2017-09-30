/*
 * scale_functions.h
 *
 * Created: 2017.09.30. 23:26:50
 *  Author: Dome
 */ 


#ifndef SCALE_FUNCTIONS_H_
#define SCALE_FUNCTIONS_H_

extern uint32_t calibr;								//kalibr�ci� be�ll�t�sa, elej�n beolvas, ezzel kappunk grammokat
extern uint32_t offset;
extern float scale;

extern uint32_t read_average(uint8_t);						//beolvas annyiszor, amennyi sz�mot adunk param�ternek, kisz�molja az �tlag�t
extern double get_value(uint8_t times);					//az �tlagb�l levonja az offsetet
extern float get_units(uint8_t times);						//get_value-t (�tlagb�l levont offsetet) m�g elosztja a scale �rt�kkel (kalibr�l� �rt�k)
extern void tare(uint8_t);									//t�r�z
extern void set_offset(long new_offset);
extern void set_scale(long new_scale);

#endif /* SCALE_FUNCTIONS_H_ */