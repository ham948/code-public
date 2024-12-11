#include  <avr/io.h>
#define   F_CPU   16000000L
#include  <util/delay.h>
#include "serialtest.h"
#include "rtctest.h"
#include "i2c.h"

//**************************************************************************** */
int main(void){
  unsigned char year, month, day, hour, min, sec;

  rtc_init();
  //rtc_setTime(0X20,0X37,0X30);
  //rtc_setDate(0X24,0X11,0X17);

  usart_init();

  while(1){
    rtc_getDate(&year,&month,&day);
    usart_send('2');
    usart_send('0');
    usart_sendPackedBCD(year);
    usart_send('/');
    usart_sendPackedBCD(month);
    usart_send('/');
    usart_sendPackedBCD(day);
    usart_send(' ');

    rtc_getTime(&hour,&min,&sec);
    usart_sendPackedBCD(hour);
    usart_send(':');void i2c_stop();
    usart_sendPackedBCD(min);
    usart_send(':');
    usart_sendPackedBCD(sec);
    usart_send('\n');
    usart_send('\r');

    _delay_ms(200);
  }
}
