#ifndef I2C_H
#define I2C_H

void i2c_stop();
void i2c_start(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char ackVal);
void i2c_init(void);

#endif