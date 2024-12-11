#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "ssd1306x32.h"
#include "serialtest.h"



int main(void){
    i2c_init();
    lcd_init();
    usart_init();
    lcd_clearFullDisplay();
    lcd_setCursor(0,0);

    lcd_printStr("WAKE UP!!!",'t');
    lcd_setCursor(0,2);
    lcd_printStr("!!!WAKE UP",'t');
    lcd_setCursor(0,5);
    lcd_printStr(" 08:15:31 ",'t');

    lcd_write_cmd(SCROLL_ON);
    lcd_setCursor(0,0);

}



    /*
    print12x16('2');
    print12x16('4');
    print12x16('/');
    print12x16('1');
    print12x16('1');
    print12x16('/');
    print12x16('2');
    print12x16('4');
    */
    //lcd_printStr("HELLO");
    //lcd_setCursor(0,2);
    //lcd_printStr("JOEY!!");
    //lcd_line(0,0,127,0);
    //lcd_line(0,8,127,8);
    //lcd_line(0,16,127,16);
    //lcd_line(0,24,127,24);
    //lcd_line(0,31,127,31);
    //lcd_square(0,0,127,31);

    //lcd_square(5,5,25,25);

    //lcd_clear_data(0)

/*
    while(1){
    lcd_setCursor(0,0);
    lcd_printStr("abcdefghij",'t');
    lcd_setCursor(0,2);
    lcd_printStr("klmnopqrst",'t');
    _delay_ms(1000);
    lcd_clearFullDisplay();
    lcd_setCursor(0,0);
    lcd_printStr("uvwxyzABCD",'t');
    lcd_setCursor(0,2);
    lcd_printStr("EFGHIJKLMN",'t');
    _delay_ms(1000);
    lcd_clearFullDisplay();
    lcd_setCursor(0,0);
    lcd_printStr("OPQRSTUVWX",'t');
    lcd_setCursor(0,2);
    lcd_printStr("YZ12345678",'t');
    _delay_ms(1000);
    lcd_clearFullDisplay();
    lcd_setCursor(0,0);
    lcd_printStr("90!#$%&'()",'t');
    lcd_setCursor(0,2);
    lcd_printStr("/:;<=>?@[]",'t');
    _delay_ms(1000);
    lcd_clearFullDisplay();
    }
*/

    //lcd_setCursor(0,3);
    //lcd_printStr("-./0123456789:;<",'m');
/*
    lcd_printStr("abcdefghilkjmnop",'m');
    lcd_setCursor(0,1);
    lcd_printStr("qrstuvwxyz123456",'m');
    lcd_setCursor(0,2);
    lcd_printStr("7890!\"#$%&'()*+,",'m');
    lcd_setCursor(0,3);
    lcd_printStr("-./0123456789:;<",'m');
    lcd_setCursor(0,4);
    lcd_printStr("=>?@ABCDEFGHIJKL",'m');
    lcd_setCursor(0,5);
    lcd_printStr("MNOPQRSTUVWXYZ[]",'m');
    lcd_setCursor(0,6);
    lcd_printStr("\\^_`{|}~27/11/24",'m');
    lcd_setCursor(0,7);
    lcd_printStr("MISS YOU VINI<3",'m');
    */
    
    /*
    while(1){
    usart_sendStr("you entered:");
    usart_send(usart_receive());
    usart_sendStr("<\n\r");
    }
    */
    //lcd_setCursor(0,3);
    //lcd_printStr("/:;<=>?@[]_^ABCD",'m');
    //lcd_printStr("QRSTUVWXYZ7890+*",'m');

    /*
    lcd_printStr(" abcdef",'t');
    lcd_square(85,16,125,31);
    lcd_square(90,20,120,26);
    lcd_square(95,22,115,24);
    */

    /*
    for(unsigned char i = 0; i < 9; i++){
    lcd_setCursor(i*8,i);

    lcd_write_data(0B00000001);
    lcd_write_data(0B00000010);
    lcd_write_data(0B00000100);
    lcd_write_data(0B11111000);
    lcd_write_data(0B00010000);
    lcd_write_data(0B00100000);
    lcd_write_data(0B01000000);
    lcd_write_data(0B10000000);
    

    }
    */
    
    /*
    while(1){
        lcd_write_cmd(NML_DSP_CMD);
        _delay_ms(75);
        lcd_write_cmd(INVERT_CMD);
        _delay_ms(75);
    }
    */
    

