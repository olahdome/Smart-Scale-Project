/*
 * HC05.h
 *
 * Created: 2017.03.26. 20:41:08
 *  Author: Dome
 */ 


#ifndef HC05_H_
#define HC05_H_

char hc_05_buffer1[25], hc_05_buffer2[50];
char temp;

void hc_05_bluetooth_transmit_byte(char data_byte);
char hc_05_bluetooth_receive_byte(void);
void hc_05_bluetooth_transmit_string(char *transmit_string);
void hc_05_bluetooth_transmit_command(char *transmit_string);
char *hc_05_bluetooth_receive_string(char *receive_string, char terminating_character);

#endif /* HC05_H_ */