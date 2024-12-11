#ifndef SERIAL_LIB_H
#define SERIAL_LIB_H

void usart_init(void);
void usart_send(unsigned char ch);
void usart_sendPackedBCD(unsigned char data);
void usart_sendStr(char s[]);
char usart_receive(void);
void usart_recieveStr(char *s);
void reverse(char s[]);
void usart_sendInt(int n);

//char *usart_receiveStr(void);

#endif