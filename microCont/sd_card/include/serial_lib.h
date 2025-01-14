#ifndef SERIAL_LIB_H
#define SERIAL_LIN_H

void UART_init(uint16_t ubrr);
void UART_putc(unsigned char c);
void UART_puts(char* s);
void UART_puthex8(uint8_t val);
void UART_puthex16(uint16_t val);
void UART_putU8(uint8_t val);
void UART_putS8(int8_t val);
void UART_putU16(uint16_t val);
void UART_putS16(int16_t val);
char UART_getc(void);
void UART_getLine(char* buf, uint8_t n);

#endif