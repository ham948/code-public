#ifndef I2C_H
#define I2C_H

void i2c_stop();
void i2c_start(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char ackVal);
void i2c_writeMulti(unsigned char bytes[],unsigned char no_b, char mem_adrs, unsigned char dev_adrs);
void i2c_readMulti(unsigned char *bytes[],unsigned char no_b, char mem_adrs, unsigned char dev_adrs);
void i2c_init(void);

#endif