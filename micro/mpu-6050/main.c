#include <stdio.h>
#include <avr/io.h>
//#include <avr/iom328p.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "i2c.h"
#include "serialtest.h"

void mpu_gyro(unsigned char* x_h,unsigned char* x_l,unsigned char loc);
void mpu_init(void);
void mpu_gyro_update(void);
void itoa2(int n, unsigned char s[]);
void reverse(unsigned char s[]);

void timer_set(void);
unsigned char x_str[] = "00000", x_gyro_h, x_gyro_l;
int x_gyro, x_gyro_old, x_gyro_diff,x_final;

int main(){
    timer_set();
    mpu_init();
    _delay_ms(200);
    usart_init();
    usart_send('t');
    while(1){
        //mpu_gyro(&x_gyro_h,&x_gyro_l,0X43);

        x_final +=x_gyro_diff;
        itoa2(x_final,x_str);
        //mpu_gyro_update();
        
        usart_send('t');
        usart_send(':');
    
        usart_send(x_str[0]);
        usart_send(x_str[1]);
        usart_send(x_str[2]);
        usart_send(x_str[3]);
        usart_send(x_str[4]);

        usart_send('\n');
        usart_send('\r');
        
        
        _delay_ms(200);
    }

}

void timer_set(void){
    TCNT1H  =    0X3D;
    TCNT1L  =    0X08;
    TCCR1A  =    0X00;
    TCCR1B  =    0X03;
    TIMSK1  =    (1<<TOIE1);
    sei();
}

ISR(TIMER1_OVF_vect){
    TCNT1H = 0X3D;
    TCNT1L = 0X08;
    mpu_gyro_update();
    //usart_send('\n');
    //usart_send('\r');


}

void mpu_init(void){
    i2c_init();
    i2c_start();
    i2c_write(0X68<<1); //shift left and leave bit 0 for write
    i2c_write(0X0E); //enable register address                    <<<<wrong
    i2c_write(0X00); //enable temp and gyro out
    i2c_stop();

    i2c_start();
    i2c_write(0X68<<1);
    i2c_write(0X6B); //clear sleep and set clock
    i2c_write(0X01);
    i2c_stop();

    i2c_start();
    i2c_write(0X68<<1);
    i2c_write(0X1B);
    i2c_write(0XE0);// self test <<<wrong its left on????
    i2c_stop();

    i2c_start();
    i2c_write(0X68<<1);
    i2c_write(0X23); // why are you writing to FIFo enable?
    i2c_write(0X04);
    i2c_stop();

}

void mpu_gyro(unsigned char *x_h,unsigned char *x_l,unsigned char loc){
    i2c_start();
    i2c_write(0X68<<1);
    i2c_write(loc);
    i2c_stop();

    i2c_start();
    i2c_write(0X68<<1 | 0x01);
    *x_h = i2c_read(1);
    *x_l = i2c_read(0);
    x_gyro = *x_h<<8 | *x_l;
    i2c_stop();
}

void mpu_gyro_update(void){
    /*
    usart_send('\n');
    usart_send('*');
    usart_send('\n');
    */
    x_gyro_old = x_gyro;

    mpu_gyro(&x_gyro_h,&x_gyro_l,0X43);

    x_gyro_diff = x_gyro - x_gyro_old;
    //x_gyro_diff = ((x_gyro_h - x_gyro_h_old)<<8) | (x_gyro_l-x_gyro_l_old);
}

void itoa2(int n, unsigned char s[]){
    int i = 0, sign; //h = 0;
    //h = (n_h<<8) | n_l;
    if((sign = n) < 0){
        n = -n;
        s[i++] = '-';
    }else
        s[i++] = ' ';
    do{
        s[i++] = n % 10 + '0';
    }while((n /= 10) > 0);
    while(i < 6){
        s[i++] = '0';
    }
    s[i] = '\0';
    reverse(s);
}

void reverse(unsigned char s[]){
    int c, i, j;

    for(i = 0, j = 4; i < j; i++, j--){
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

