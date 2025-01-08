#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "rtctest.h"
#include "serialtest.h"
#include "twi.h"
#include "ssd1306.h"

int main(void){
	unsigned char year, month, day, hour, min, sec;

	rtc_init();
	rtc_setTime(0x12,0x37,0x30);
	//rtc_setDate(0x24,0x11,0x17);
	
	usart_init();
	SSD1306_Init (SSD1306_ADDR);


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
    		usart_send(':');
    		usart_sendPackedBCD(min);
    		usart_send(':');
    		usart_sendPackedBCD(sec);
    		usart_send('\n');
    		usart_send('\r');
		
		SSD1306_ClearScreen();
		SSD1306_SetPosition(40,0);
		
		SSD1306_DrawChar('0' + (hour>>4));
                SSD1306_DrawChar('0' + (hour&0x0F));
		SSD1306_DrawChar(':');
		SSD1306_DrawChar('0' + (min>>4));
                SSD1306_DrawChar('0' + (min&0x0F));
		SSD1306_DrawChar(':');  
		SSD1306_DrawChar('0' + (sec>>4));
		SSD1306_DrawChar('0' + (sec&0x0F));
		SSD1306_UpdateScreen(SSD1306_ADDR);

		_delay_ms(200);
	}
}
