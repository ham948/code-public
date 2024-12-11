#include <avr/io.h>
#include <i2c.h>
//----------------------------------------------------------------------

void rtc_init(void){
  i2c_init();
  i2c_start();
  i2c_write(0XD0);  //address for RTC to write (0XD1 for read) bit7-1 is address, bit 0 for R/W
  i2c_write(0X07);
  i2c_write(0X00);
  i2c_stop();

  //i2c_start();
  //i2c_write(0XD0);
  //i2c_write(0X02);
  //i2c_write(0X00);
  //i2c_stop();
}
void rtc_setTime(unsigned char h,unsigned char m,unsigned char s){
  i2c_start();
  i2c_write(0XD0);//address
  i2c_write(0); //memory location to start writing
  i2c_write(s);
  i2c_write(m);
  i2c_write(h);
  i2c_stop();
}
void rtc_setDate(unsigned char y,unsigned char m,unsigned char d){
  i2c_start();
  i2c_write(0XD0);
  i2c_write(0X04);
  i2c_write(d);
  i2c_write(m);
  i2c_write(y);
  i2c_stop();
}

void rtc_getTime(unsigned char *h, unsigned char *m, unsigned char *s){
  i2c_start();
  i2c_write(0XD0);
  i2c_write(0);
  i2c_stop();

  i2c_start();
  i2c_write(0XD1);   // address for read
  *s = i2c_read(1);
  *m = i2c_read(1);
  *h = i2c_read(0);
  i2c_stop();

}

void rtc_getDate(unsigned char *y, unsigned char *m, unsigned char *d){
  i2c_start();
  i2c_write(0XD0);
  i2c_write(0X04);
  i2c_stop();

  i2c_start();
  i2c_write(0XD1);
  *d = i2c_read(1);
  *m = i2c_read(1);
  *y = i2c_read(0);
  i2c_stop();

}