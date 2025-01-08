#include <avr/io.h>
#include <avr/interrupt.h>
#include "ssd1306x32.h"
#include "i2c.h"
#include <util/delay.h>

void printTime(unsigned char h,unsigned char m);
void printSecs(unsigned char s);
void printDate(unsigned char y,unsigned char m,unsigned char d);
void printTemp(unsigned char t);
void rtc_init(void);

void getTime(unsigned char *h,unsigned char *m);
void getAlarm(unsigned char *h,unsigned char *m);
void getSec(unsigned char *s);
void getDate(unsigned char *d,unsigned char *m,unsigned char *y);
void getTemp(unsigned char *t);
void setTime(unsigned char h,unsigned m);
void setAlarm(unsigned char h,unsigned char m);
void setDate(unsigned char d,unsigned char m,unsigned char y);
void setTimeMenu(void);
void setDateMenu(void);
void setAlarmMenu(void);

void normalDisplay(void);
void alarmToggle(void);
void timerInit(void);
void bttnInit(void);


unsigned char seconds, hours, minutes, date, month, year, temp;
unsigned char alarm_state = 0, btn0_state = 0, btn1_state = 0;
unsigned char btn0_short_state = 0,btn0_long_state = 0,btn1_short_state = 0,btn1_long_state = 0;

int main(void){
    i2c_init();
    lcd_init();
    timerInit();
    bttnInit();
    sei();

    lcd_clearFullDisplay();

    while(1){
    normalDisplay();
    if(btn0_short_state == 1)
        alarmToggle();
    if(btn1_long_state == 1)
        setTimeMenu();
    if(btn0_long_state == 1)
        setAlarmMenu();
    }
}

void normalDisplay(void){
        _delay_ms(200);
        getTime(&hours,&minutes);
        getDate(&date,&month,&year);
        getSec(&seconds);
        getTemp(&temp);
        printTime(hours,minutes);
        printDate(date,month,year);
        lcd_setCursor(4,4);
        printSecs(seconds);
        printTemp(temp);
}

void timerInit(void){
    TCCR1A = 0X00;
    TCCR1B = 0X0D;

    OCR1A = 46875;
    TIMSK1 = (1<<OCIE1A);
}

ISR(TIMER1_COMPA_vect){
    if(btn0_state == 1){
        btn0_long_state = 1;
    }
    if(btn1_state == 1){
        btn1_long_state = 1;
    }
}

void bttnInit(void){
    PORTD = (1<<2) | (1<<3);
    EICRA = (1<<ISC00) | (1<<ISC10);
    EIMSK = (1<<INT0)  | (1<<INT1);
}

ISR(INT0_vect){
    TCNT1H = 0;
    TCNT1L = 0;
    
    if(btn0_state == 0){
    _delay_ms(50);
    if(!(PIND & (1<<2)))
        btn0_state = 1;

    }else{
    _delay_ms(50);
    if((PIND & (1<<2)))
        btn0_state = 0;
    if(!btn0_long_state)
        btn0_short_state = 1;
    }

}

ISR(INT1_vect){
    TCNT1H = 0;
    TCNT1L = 0;

    if(btn1_state == 0){
    _delay_ms(50);
    if(!(PIND & (1<<3)))
        btn1_state = 1;

    }else{
    _delay_ms(50);
    if((PIND & (1<<3)))
        btn1_state = 0;
    if(!btn1_long_state)
        btn1_short_state = 1;
    }
}

void printTime(unsigned char h,unsigned char m){
    lcd_setCursor(4,0);
    lcd_print25x32('0' + (h>>4));
    lcd_print25x32('0' + (h & 0X0F));
    printColon();
    lcd_print25x32('0' + (m>>4));
    lcd_print25x32('0' + (m & 0X0F));
}

void printSecs(unsigned char s){
    lcd_setCursor(4,4);
    lcd_print25x32('0' + (s>>4));
    lcd_print25x32('0' + (s & 0X0F));
}

void printDate(unsigned char y,unsigned char m,unsigned char d){
    lcd_setCursor(54,4);
    lcd_print10x16('0' + (y>>4));
    lcd_print10x16('0' + (y & 0X0F));

    for(int i = 0; i<5;i++){
        if(i == 2){
        lcd_write_data(0XFF);
        lcd_movCursor(0,1);
        lcd_write_data(0XFF);
        }else{
        lcd_write_data(0X00);
        lcd_movCursor(0,1);
        lcd_write_data(0X00);
        }
        lcd_movCursor(1,-1);
    }

    lcd_print10x16('0' + (m>>4));
    lcd_print10x16('0' + (m & 0X0F));

    for(int i = 0; i<5;i++){
        if(i == 2){
        lcd_write_data(0XFF);
        lcd_movCursor(0,1);
        lcd_write_data(0XFF);
        }else{
        lcd_write_data(0X00);
        lcd_movCursor(0,1);
        lcd_write_data(0X00);
        }
        lcd_movCursor(1,-1);
    }

    lcd_print10x16('0' + (d>>4));
    lcd_print10x16('0' + (d & 0X0F));
}

void printTemp(unsigned char t){

    unsigned char lower, higher;
    higher = t/10;
    lower = t - higher*10;
    lcd_setCursor(79,6);
    lcd_print10x16('0' + higher);
    lcd_print10x16('0' + lower);
    lcd_movCursor(5,0);
    lcd_print10x16(':');
}

void getTime(unsigned char *h,unsigned char *m){
    unsigned char time[2];
    i2c_readMulti(time,2,0X01,0X68);
    *m = time[0];
    *h = time[1] & 0X3F;
}

void getAlarm(unsigned char *h, unsigned char *m){
    unsigned char aTime[2];
    i2c_readMulti(aTime,2,0X08,0X68);
    *m = aTime[0];
    *h = aTime[1] & 0X3F;
}

void getSec(unsigned char *s){
    i2c_readMulti(s,1,0X00,0X68);
}

void getTemp(unsigned char *t){
  i2c_start();
  i2c_write(0XD0);
  i2c_write(0X11);
  i2c_stop();

  i2c_start();
  i2c_write(0XD1);
  *t = i2c_read(0);
  i2c_stop();
}

void getDate(unsigned char *d,unsigned char *m,unsigned char *y){
    unsigned char date[3];
    i2c_readMulti(date,3,0X04,0X68);
    *d = date[0];
    *m = date[1];
    *y = date[2];
}

void setTime(unsigned char h,unsigned m){
    unsigned char time[3];
    time[0] = 0X00;
    time[1] = m;
    time[2] = 0XBF & h;
    i2c_writeMulti(time,3,0X00,0X68);
}

void setAlarm(unsigned char h,unsigned char m){
    unsigned char aTime[2];
    aTime[0] = m;
    aTime[1] = 0XBF & h;
    i2c_writeMulti(aTime,2,0X08,0X68);

    i2c_write_byte(0X80,0X0A,0X68); //set alarm to go off when s,m and hours match
}

void setDate(unsigned char d,unsigned char m,unsigned char y){

    unsigned char date[3];
    date[0] = d;
    date[1] = m;
    date[2] = y;
    i2c_writeMulti(date,3,0X04,0X68);
}

void setTimeMenu(void){
    btn1_long_state = 0;
    unsigned char newHour = 0X00, newMinute = 0X00;
    unsigned char timeArray[4];
    unsigned char curserPos = 0;
    printSecs(0X00);

    getTime(&newHour,&newMinute);
    timeArray[0] = newHour>>4;
    timeArray[1] = newHour&0X0F;
    timeArray[2] = newMinute>>4;
    timeArray[3] = newMinute&0X0F;

    while(curserPos < 4){
    lcd_setCursor(4,0);
    for(int i = 0;i<4;i++){
        if(curserPos == i) lcd_invertPrint(1);
        lcd_print25x32('0' + timeArray[i]);
        if(curserPos == i) lcd_invertPrint(0);
        if(i == 1) printColon();
    }

    if(btn1_short_state){
        unsigned char limits[] = {2,9,6,9};
        for(int i =0; i<4; i++){
            if(curserPos == i){
                if(timeArray[curserPos] < limits[i])
                    timeArray[curserPos]++;
                else
                    timeArray[curserPos] = 0;
            }
        }
        btn1_short_state = 0;
    }
    if(btn0_short_state){
        curserPos++;
        btn0_short_state = 0;
    }
    }
    newHour = timeArray[0]<<4 | timeArray [1];
    newMinute = timeArray [2]<<4 | timeArray [3];
    setTime(newHour,newMinute);
    printTime(newHour,newMinute);

    setDateMenu();
}

void setAlarmMenu(void){
    btn0_long_state = 0;
    btn1_short_state = 0;
    unsigned char newAHour = 0X00, newAMinute = 0X00;
    unsigned char timeArrayA[4];
    unsigned char curserPos = 0;
    lcd_clearFullDisplay();

    getAlarm(&newAHour,&newAMinute);
    timeArrayA[0] = newAHour>>4;
    timeArrayA[1] = newAHour&0X0F;
    timeArrayA[2] = newAMinute>>4;
    timeArrayA[3] = newAMinute&0X0F;
    //_delay_ms(1000);
    while(curserPos < 4){
    lcd_setCursor(4,0);
    for(int i = 0;i<4;i++){
        if(curserPos == i) lcd_invertPrint(1);
        lcd_print25x32('0' + timeArrayA[i]);
        if(curserPos == i) lcd_invertPrint(0);
        if(i == 1) printColon();
    }
    

    if(btn1_short_state){
        unsigned char limits[] = {2,9,6,9};
        for(int i =0; i<4; i++){
            if(curserPos == i){
                if(timeArrayA[curserPos] < limits[i])
                    timeArrayA[curserPos]++;
                else
                    timeArrayA[curserPos] = 0;
            }
        }
        btn1_short_state = 0;
    }
    if(btn0_short_state){
        curserPos++;
        btn0_short_state = 0;
    }
    }
    newAHour = timeArrayA[0]<<4 | timeArrayA [1];
    newAMinute = timeArrayA[2]<<4 | timeArrayA [3];
    setAlarm(newAHour,newAMinute);
}

void setDateMenu(void){
    unsigned char newDate, newMonth, newYear;
    unsigned char dateArray[6];
    unsigned char curserPos = 0;

    getDate(&newDate,&newMonth,&newYear);

    dateArray[0] = newDate  >> 4;
    dateArray[1] = newDate & 0X0F;
    dateArray[2] = newMonth >> 4;
    dateArray[3] = newMonth & 0X0F;
    dateArray[4] = newYear  >> 4;
    dateArray[5] = newYear & 0X0F;

    while(curserPos < 6){
        lcd_setCursor(54,4);

        for(int i=0;i<6;i++){
            if(i == curserPos) lcd_invertPrint(1);
            lcd_print10x16('0' + (dateArray[i]));
            if(i == curserPos) lcd_invertPrint(0);

            if(i == 1 || i == 3){
                for(int i = 0; i<5;i++){
                    if(i == 2){
                    lcd_write_data(0XFF);
                    lcd_movCursor(0,1);
                    lcd_write_data(0XFF);
                    }else{
                    lcd_write_data(0X00);
                    lcd_movCursor(0,1);
                    lcd_write_data(0X00);
                    }
                    lcd_movCursor(1,-1);
                }
            }
        }

        if(btn0_short_state){
            curserPos++;
            btn0_short_state = 0;
        }
        if(btn1_short_state){
            unsigned char limits[] = {3,9,1,9,9,9};
            for(int i = 0; i<6 ; i++){
                if(curserPos == i){
                    if(dateArray[curserPos] < limits[i])
                        dateArray[curserPos]++;
                    else{
                        if(!(curserPos == 1))
                            dateArray[curserPos] = 0;
                        else
                            dateArray[curserPos] = 1;
                    }
                }
            }
            btn1_short_state = 0;
        }
    }

    newDate  = (dateArray[0]<<4) | dateArray[1];
    newMonth = (dateArray[2]<<4) | dateArray[3];
    newYear  = (dateArray[4]<<4) | dateArray[5];
    setDate(newDate,newMonth,newYear);

}

void alarmToggle(void){

    if(alarm_state == 0){ //alarm on procedure
    lcd_line(0,0,0,63);
    lcd_line(1,0,1,63);
    lcd_line(126,0,126,63);
    lcd_line(127,0,127,63);

    i2c_write_byte(0B00011101,0X0E,0X68);

    alarm_state = 1;
    }
    else if(alarm_state == 1){ //alarm off procedure
        lcd_setCursor(0,0);
        for(int i = 0;i<8;i++){
            lcd_clear_data();
            lcd_movCursor(-1,1);
        }
        lcd_setCursor(1,0);
        for(int i = 0;i<8;i++){
            lcd_clear_data();
            lcd_movCursor(-1,1);
        }
        lcd_setCursor(126,0);
        for(int i = 0;i<8;i++){
            lcd_clear_data();
            lcd_movCursor(-1,1);
        }
        for(int i = 0;i<8;i++){
            lcd_setCursor(127,i);
            lcd_clear_data();
        }
    
    i2c_write_byte(0B00011100,0X0E,0X68);
    alarm_state = 0;
    }
    btn0_short_state = 0;
}