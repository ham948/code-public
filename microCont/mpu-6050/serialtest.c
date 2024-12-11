#include  <avr/io.h>
//#include <avr/iom328p.h>
//#include "serialtest.h"

void usart_init(void){
  UCSR0B = (1<<TXEN0);
  UCSR0C = (1<<UCSZ01)| (1<<UCSZ00);
  UBRR0L = 103; 
}

void usart_send(unsigned char ch){
  while(!(UCSR0A&(1<<UDRE0)));
  UDR0 = ch;
}

void usart_sendPackedBCD(unsigned char data){
  usart_send('0' + (data>>4));
  usart_send('0' + (data & 0X0F));
}
