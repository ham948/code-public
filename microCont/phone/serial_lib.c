#include  <avr/io.h>
#include "serial_lib.h"

void usart_init(void){
  UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0);  //rx and tx enable AND rx interrupt
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

void usart_recieveStr(char *s){
  int i;
  char c = ' ';
  UCSR0B ^= (1<<RXCIE0);

  do{
    c = usart_receive();
  }while(c == '\r' || c == '\n');
  s[0][0] = c;
  //while((s[0] == '\n') || (s[0] = usart_receive()) == '\r'); //skips carriage return and assigns \n to s[0]
  for(i = 1;(s[0][i]=usart_receive()) != '\r';i++);    //use \0 if between devices, but Putty uses \r when you hit enter
  s[0][i] = '\0';

  do{
    c = usart_receive();
  }while(c == '\r' || c == '\n');
  s[1][0] = c;
  for(i = 1;(s[1][i]=usart_receive()) != '\r';i++);
  s[1][i] = '\0';


  UCSR0B ^= (1<<RXCIE0);
}
/*
char *usart_receiveStr(void){
  char in_str[100];
  for(unsigned char i = 0;UDR0 != '\n';i++)
    in_str[i] = usart_receive();
  return in_str;
}
*/

void usart_sendInt(int n){
  char out[] = "+00000";
  unsigned char i = 0, sign = 1;
  if(n < 0){
    sign = 0;
    n = -n;
  }
  do{
    out[i++] = n % 10 + '0';
  }while((n/=10)>0);
  while(i<5)
    out[i++] = '0';
  out[i++] = (sign)? '+' : '-';
  reverse(out);
  usart_sendStr(out);
}

void reverse(char s[]){
    int c, i, j;

    for(i = 0, j = 5; i < j; i++, j--){
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}