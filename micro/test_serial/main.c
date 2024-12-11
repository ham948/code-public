#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "serial_lib.h"

int main(void){
    usart_init();

    char fromSIMstr[50];

    usart_recieveStr(fromSIMstr);
    _delay_ms(500);
    usart_sendStr(fromSIMstr);

}
// \r\nhelloworld\r\n