#include <stdio.h>
#include <math.h>
#include <avr/io.h>
//#include <avr/iom328p.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "ssd1306x32.h"
#include "i2c.h"
#include "serial_lib.h"

void mpu_init(void);
void mpu_gyro_cal(void);
void calc_IMU_error(void);
void mpu_read_xg(int *out);
void mpu_read_yg(int *out);
void mpu_read_zg(int *out);
void gyro_pos(float *x,float *y,float *z);
void mpu_read_FT_x(unsigned char *out);
void mpu_read_temp(float *temp);
void timer_set(void);

float xg_offset, yg_offset, zg_offset;

int miliCnt = 0;

int main(void){
    i2c_init();
    lcd_init();
    //usart_init();
    mpu_init();
    timer_set();

    float   gyro_x_abs = 0, gyro_y_abs = 0, gyro_z_abs = 0;


    lcd_clearFullDisplay();
    lcd_setCursor(0,0);
    while(1){
        
        gyro_pos(&gyro_x_abs,&gyro_y_abs,&gyro_z_abs);
        lcd_printStr("X: ",'t');
        lcd_printInt(gyro_x_abs,'t');

        lcd_setCursor(0,2);
        lcd_printStr("Y: ",'t');
        lcd_printInt(gyro_y_abs,'t');

        lcd_setCursor(0,4);
        lcd_printStr("Z: ",'t');
        lcd_printInt(gyro_z_abs,'t');
        lcd_setCursor(0,0);

        //_delay_ms(40);

    }

}

void mpu_init(void){

    i2c_write_byte(0X01,0X6B,0X68); //clear reset to turn it on
    i2c_write_byte(0X00,0X1B,0X68); // turn off test and set range to 250degrees/sec
    _delay_ms(100);
    mpu_gyro_cal();


}

void gyro_pos(float *x,float *y,float *z){
    static int     old_cnt = 0, new_cnt = 0; //declaring these static is so important
    static int     gyro_x,         gyro_y,         gyro_z;
    static float cnt_diff = 0;

    old_cnt = new_cnt;
    mpu_read_xg(&gyro_x);
    mpu_read_yg(&gyro_y);
    mpu_read_zg(&gyro_z);
    new_cnt = miliCnt;

    if(!cnt_diff) cnt_diff = new_cnt - old_cnt;
    cnt_diff = (cnt_diff+(new_cnt - old_cnt))/2;

    *x += (((gyro_x-xg_offset)*cnt_diff)/131)/1000;
    *y += (((gyro_y-yg_offset)*cnt_diff)/131)/1000;
    *z += (((gyro_z-zg_offset)*cnt_diff)/131)/1000;
}


void mpu_gyro_cal(void){
    int tmp_x,tmp_y,tmp_z;
    mpu_read_xg(&tmp_x);
    mpu_read_yg(&tmp_y);
    mpu_read_zg(&tmp_z);
    xg_offset = tmp_x;
    yg_offset = tmp_y;
    zg_offset = tmp_z;

    for(int i=0;i<400;i++){
        mpu_read_xg(&tmp_x);
        mpu_read_yg(&tmp_y);
        mpu_read_zg(&tmp_z);

        xg_offset = (xg_offset+tmp_x)/2.0;
        yg_offset = (yg_offset+tmp_y)/2.0;
        zg_offset = (zg_offset+tmp_z)/2.0;

        _delay_ms(5);
    }

}



void mpu_read_xg(int *out){
    unsigned char x[2];
    i2c_readMulti(x,2,0X43,0X68);
    *out = x[0]<<8 | x[1];
}

void mpu_read_yg(int *out){
    unsigned char y[2];
    i2c_readMulti(y,2,0X45,0X68);
    *out = y[0]<<8 | y[1];
}

void mpu_read_zg(int *out){
    unsigned char z[2];
    i2c_readMulti(z,2,0X47,0X68);
    *out = z[0]<<8 | z[1];
}

void mpu_read_FT_x(unsigned char *out){

    i2c_start();
    i2c_write(0X68<<1);
    i2c_write(0X0D); //address of x gyro self test
    i2c_stop();

    i2c_start();
    i2c_write(0X68<<1 | 0x01); //read
    *out = 0X1F & i2c_read(0);
    i2c_stop();
}

void mpu_read_temp(float *temp){
    unsigned char t_byte[2];
    float t;
    i2c_readMulti(t_byte,2,0X42,0X68);

    t = t_byte[1]<<8 | t_byte[0];
    *temp = (t/340) + 36.53;
}

void timer_set(void){
    //every 1ms;
    TCNT1H = 0xC1;
    TCNT1L = 0X80;

    TCCR1A = 0X00;
    TCCR1B = 0X01; // no prescaller
    TIMSK1 = (1<<TOIE1);
    sei();
}
ISR(TIMER1_OVF_vect){

    TCNT1H = 0xC1;
    TCNT1L = 0X80;

    miliCnt++;
}

void calc_IMU_error(void){
    int x_preTest = 0, x_postTest = 0;//, self_test_r = 0;
    unsigned char XG_test = 0;
    for(int i=0;i<100;i++){
        unsigned char tmp;
        tmp = x_preTest;
        mpu_read_xg(&x_preTest);
        x_preTest = (x_preTest+tmp)/2;
    }
    
    i2c_start();
    i2c_write(0X68<<1);
    i2c_write(0X1B); //self test on
    i2c_write(0X80);
    i2c_stop();


    for(int i=0;i<100;i++){
        unsigned char tmp;
        tmp = x_postTest;
        mpu_read_xg(&x_postTest);
        x_postTest = (x_postTest+tmp)/2;
    }

    mpu_read_FT_x(&XG_test);   //not used yet need to finish!!!!!

    i2c_start();
    i2c_write(0X68<<1);
    i2c_write(0X1B); //self test off
    i2c_write(0X00);
    i2c_stop();

    //self_test_r = x_postTest - x_preTest;

    
}