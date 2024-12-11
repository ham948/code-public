#ifndef MAIN_H
#define MAIN_H

void usart_init(void);
void usart_send(unsigned char ch);
void usart_sendPackedBCD(unsigned char data);

#endif