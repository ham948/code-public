#include <avr/io.h>
#include <util/delay.h>
#include "keypad4x4.h"

unsigned char colloc, rowloc;

unsigned char keyPad[4][4] = {{'1','2','3','A'},
                              {'4','5','6','B'},
                              {'7','8','9','C'},
                              {'*','0','#','D'}};

void keypad_init(void){
    KEY_DDR1 = (0X30 & 0XF0)<<2;
    KEY_DDR2 = 0XF0>>6;

    KEY_PRT1 = (0X30 & 0XF0)<<2;
    KEY_PRT2 = 0XFF>>6;
}

char keypad_read(void){
    unsigned char out = ' ';

        do{
            KEY_PRT1 &= 0X0F<<2;
            KEY_PRT2 &= 0X0F>>6;
            __asm("NOP");
            colloc = (KEY_PIN1>>2 & 0X0F);
        }while(colloc != 0x0F);
        
        do{
            do{
                _delay_ms(80);
                colloc = (KEY_PIN1>>2 & 0X0F);
            }while(colloc == 0X0F);
            _delay_ms(80);
            colloc = (KEY_PIN1>>2 & 0X0F);
        }while(colloc == 0X0F);

        while(1){
            KEY_PRT1 = (0X3F &0XEF)<<2;
            KEY_PRT2 = 0XEF>>6;
            __asm("NOP");
            colloc = (KEY_PIN1>>2 & 0X0F);
            if(colloc != 0X0F){
                rowloc = 0;
                break;
            }

            KEY_PRT1 = (0X3F & 0XDF)<<2;
            KEY_PRT2 = 0XDF>>6;
            __asm("NOP");
            colloc = (KEY_PIN1>>2 & 0X0F);
            if(colloc != 0X0F){
                rowloc = 1;
                break;
            }

            KEY_PRT1 = (0X3F & 0XBF)<<2;
            KEY_PRT2 = 0XBF>>6;
            __asm("NOP");
            colloc = (KEY_PIN1>>2 & 0X0F);
            if(colloc != 0X0F){
                rowloc = 2;
                break;
            }

            KEY_PRT1 = (0X3F & 0X7F)<<2;
            KEY_PRT2 = 0X7F>>6;
            __asm("NOP");
            colloc = (KEY_PIN1>>2 & 0X0F);
            rowloc = 3;
            break;
        }

        if(colloc == 0X0E)
            out = keyPad[rowloc][0];
        else if(colloc == 0X0D)
            out = keyPad[rowloc][1];
        else if(colloc == 0X0B)
            out = keyPad[rowloc][2];
        else if(colloc == 0X07)
            out = keyPad[rowloc][3];

    return out;
}