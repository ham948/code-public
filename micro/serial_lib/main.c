#include <avr/io.h>
#include <util/delay.h>
#include "serial_lib.h"

int main(void){
    char a=-124;
    usart_init();
    while(1){
    usart_sendStr("hello\n\r");
    usart_sendInt(a);
    usart_sendStr("\n\r");
    _delay_ms(200);
    }

}