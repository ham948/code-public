#include  <avr/io.h>
#include "serialtest.h"

void usart_init(void){
  UCSR0B = (1<<TXEN0) | (1<<RXEN0);
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

void usart_sendStr(char s[]){
  for(unsigned i = 0; s[i] != '\0';i++)
    usart_send(s[i]);
}

char usart_receive(void){
  while(!(UCSR0A & (1<<RXC0)));
  return UDR0;
}
/*
char *usart_receiveStr(void){
  char in_str[100];
  for(unsigned char i = 0;UDR0 != '\n';i++)
    in_str[i] = usart_receive();
  return in_str;
}
*/