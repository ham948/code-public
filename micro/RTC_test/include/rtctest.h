#ifndef RTCTEST_H
#define RTCTEST_H
//-------------------------------------------------------
void rtc_init(void);
void rtc_setTime(unsigned char h,unsigned char m,unsigned char s);
void rtc_setDate(unsigned char y,unsigned char m,unsigned char d);
void rtc_getTime(unsigned char *h, unsigned char *m, unsigned char *s);
void rtc_getDate(unsigned char *y, unsigned char *m, unsigned char *d);

#endif