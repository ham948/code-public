#include <stdio.h>
#include <avr/pgmspace.h>
#include "serialtest.h"

const char PROGMEM sampleTxt[] = "hello World!!";

int main(void){
    usart_init();
    usart_send('h');
    usart_send('\n');
    usart_send('\r');
    while(1){
        for(unsigned char i = 0; pgm_read_byte(&sampleTxt[i]) != '\0' ; i++)
            usart_send(pgm_read_byte(&sampleTxt[i]));
        usart_send('\n');
        usart_send('\r');
    }
    


}