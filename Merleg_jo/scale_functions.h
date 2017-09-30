/*
 * scale_functions.h
 *
 * Created: 2017.09.30. 23:26:50
 *  Author: Dome
 */ 


#ifndef SCALE_FUNCTIONS_H_
#define SCALE_FUNCTIONS_H_

extern uint32_t calibr;								//kalibráció beállítása, elején beolvas, ezzel kappunk grammokat
extern uint32_t offset;
extern float scale;

extern uint32_t read_average(uint8_t);						//beolvas annyiszor, amennyi számot adunk paraméternek, kiszámolja az átlagát
extern double get_value(uint8_t times);					//az átlagból levonja az offsetet
extern float get_units(uint8_t times);						//get_value-t (átlagból levont offsetet) még elosztja a scale értékkel (kalibráló érték)
extern void tare(uint8_t);									//táráz
extern void set_offset(long new_offset);
extern void set_scale(long new_scale);

#endif /* SCALE_FUNCTIONS_H_ */