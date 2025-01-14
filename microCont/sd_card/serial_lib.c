#include <avr/io.h>

void UART_init(uint16_t ubrr){

    UBRR0L = (uint8_t)(ubrr&0XFF);
    UBRR0H = (uint8_t)(ubrr>>8);

    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);

}

void UART_putc(unsigned char c){
    while(!(UCSR0A & (1<<UDRE0)));

    UDR0 = c;
}

void UART_puts(char* s)
{
    // transmit character until NULL is reached
    while(*s > 0) UART_putc(*s++);
}



void UART_puthex8(uint8_t val)
{
    // extract upper and lower nibbles from input value
    uint8_t upperNibble = (val & 0xF0) >> 4;
    uint8_t lowerNibble = val & 0x0F;

    // convert nibble to its ASCII hex equivalent
    upperNibble += upperNibble > 9 ? 'A' - 10 : '0';// if its C then c-10= 2 and add that to 'A'
    lowerNibble += lowerNibble > 9 ? 'A' - 10 : '0';

    // print the characters
    UART_putc(upperNibble);
    UART_putc(lowerNibble);
}

void UART_puthex16(uint16_t val)
{
    // transmit upper 8 bits
    UART_puthex8((uint8_t)(val >> 8));

    // transmit lower 8 bits
    UART_puthex8((uint8_t)(val & 0x00FF));
}

void UART_putU8(uint8_t val)
{
    uint8_t dig1 = '0', dig2 = '0';

    // count value in 100s place
    while(val >= 100)
    {
        val -= 100;
        dig1++;
    }

    // count value in 10s place
    while(val >= 10)
    {
        val -= 10;
        dig2++;
    }

    // print first digit (or ignore leading zeros)
    if(dig1 != '0') UART_putc(dig1);

    // print second digit (or ignore leading zeros)
    if((dig1 != '0') || (dig2 != '0')) UART_putc(dig2);

    // print final digit
    UART_putc(val + '0');
}

void UART_putS8(int8_t val)
{
    // check for negative number
    if(val & 0x80)
    {
        // print negative sign
        UART_putc('-');

        // get unsigned magnitude
        val = ~(val - 1);
    }

    // print magnitude
    UART_putU8((uint8_t)val);
}

void UART_putU16(uint16_t val)
{
    uint8_t dig1 = '0', dig2 = '0', dig3 = '0', dig4 = '0';

    // count value in 10000s place
    while(val >= 10000)
    {
        val -= 10000;
        dig1++;
    }

    // count value in 1000s place
    while(val >= 1000)
    {
        val -= 1000;
        dig2++;
    }

    // count value in 100s place
    while(val >= 100)
    {
        val -= 100;
        dig3++;
    }

    // count value in 10s place
    while(val >= 10)
    {
        val -= 10;
        dig4++;
    }

    // was previous value printed?
    uint8_t prevPrinted = 0;

    // print first digit (or ignore leading zeros)
    if(dig1 != '0') {UART_putc(dig1); prevPrinted = 1;}

    // print second digit (or ignore leading zeros)
    if(prevPrinted || (dig2 != '0')) {UART_putc(dig2); prevPrinted = 1;}

    // print third digit (or ignore leading zeros)
    if(prevPrinted || (dig3 != '0')) {UART_putc(dig3); prevPrinted = 1;}

    // print third digit (or ignore leading zeros)
    if(prevPrinted || (dig4 != '0')) {UART_putc(dig4); prevPrinted = 1;}

    // print final digit
    UART_putc(val + '0');
}

void UART_putS16(int16_t val)
{
    // check for negative number
    if(val & 0x8000)
    {
        // print minus sign
        UART_putc('-');

        // convert to unsigned magnitude
        val = ~(val - 1);
    }

    // print unsigned magnitude
    UART_putU16((uint16_t) val);
}

char UART_getc(void)
{
    // wait for data
    while(!(UCSR0A & (1 << RXC0)));

    // return data
    return UDR0;
}

void UART_getLine(char* buf, uint8_t n)
{
    uint8_t bufIdx = 0;
    char c;

    // while received character is not carriage return
    // and end of buffer has not been reached
    do
    {
        // receive character
        c = UART_getc();

        // store character in buffer
        buf[bufIdx++] = c;
    }
    while((bufIdx < n) && (c != '\r'));

    // ensure buffer is null terminated
    buf[bufIdx] = 0;
}