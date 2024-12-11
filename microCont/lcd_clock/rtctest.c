#include <avr/io.h>

void i2c_stop(){
  TWCR  =   (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

void i2c_start(void){
  TWCR  =   (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
}

void i2c_write(unsigned char data){
  TWDR  =   data;
  TWCR  =   (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
}

unsigned char i2c_read(unsigned char ackVal){
  TWCR  =   (1<<TWINT) | (1<<TWEN) | (ackVal<<TWEA);
  while(!(TWCR & (1<<TWINT)));
  return TWDR;
}

void i2c_init(void){
  TWSR = 0X00;
  TWBR = 152;
  TWCR = 0X04;
}

//----------------------------------------------------------------------

void rtc_init(void){
  i2c_init();
  i2c_start();
  i2c_write(0XD0);
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
  i2c_write(0XD0);
  i2c_write(0);
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
  i2c_write(0XD1);
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