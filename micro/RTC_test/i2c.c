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