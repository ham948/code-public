#ifndef SERIALTEST_H
#define SERIALTEST_H

void usart_init(void);
void usart_send(unsigned char ch);
void usart_sendPackedBCD(unsigned char data);
void usart_sendStr(char s[]);
char usart_receive(void);
//char *usart_receiveStr(void);

#endif