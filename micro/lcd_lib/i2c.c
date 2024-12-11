#include <avr/io.h>

void i2c_stop(){
  TWCR  =   (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
  //TWINT is interrupt flag setting it to 1 allows data to be sent to data register
  //TWEN  enables the I2C (TWI)
  //TWST0 sends stop sequence (pulls data HIGH when clock high)
}

void i2c_start(void){
  TWCR  =   (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
  //TWST0 sends stop sequence (pulls data LOW when clock high)
  //wait for TWINT flag befor continuing
}

void i2c_write(unsigned char data){
  TWDR  =   data; // load data ready for send
  TWCR  =   (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
}

unsigned char i2c_read(unsigned char ackVal){
  TWCR  =   (1<<TWINT) | (1<<TWEN) | (ackVal<<TWEA);
  while(!(TWCR & (1<<TWINT)));
  return TWDR;
}

void i2c_writeMulti(unsigned char bytes[],unsigned char no_b, char mem_adrs, unsigned char dev_adrs){
  i2c_start();
  i2c_write(dev_adrs<<1);
  i2c_write(mem_adrs);
  for(unsigned char i = 0; i< no_b;i++)
    i2c_write(bytes[i]);
  i2c_stop();

}

void i2c_readMulti(unsigned char *bytes[],unsigned char no_b, char mem_adrs, unsigned char dev_adrs){
  i2c_start();
  i2c_write(dev_adrs<<1);
  i2c_write(mem_adrs);
  i2c_stop();

  i2c_start();
  i2c_write(dev_adrs<<1 | 1);
  for(unsigned char i = 0;i < no_b; i++)
    *bytes[i] = i2c_read(1);
  *bytes[no_b-1] = i2c_read(0);
  i2c_stop();

}

void i2c_init(void){
  TWSR = 0X00;  // i2c clock (scl) prescaler set to 0 (4^prescaler = 0) 
  TWBR = 152;   // scl freq. to 50k for 16MHz mcu
  TWCR = 0X04;  // enable TWI mode
}