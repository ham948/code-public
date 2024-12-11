#ifndef RTCTEST_H
#define RTCTEST_H
//-------------------------------------------------------
void i2c_stop();
void i2c_start(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char ackVal);
void i2c_init(void);
//-------------------------------------------------------
void rtc_init(void);
void rtc_setTime(unsigned char h,unsigned char m,unsigned char s);
void rtc_setDate(unsigned char y,unsigned char m,unsigned char d);
void rtc_getTime(unsigned char *h, unsigned char *m, unsigned char *s);
void rtc_getDate(unsigned char *y, unsigned char *m, unsigned char *d);

#endif