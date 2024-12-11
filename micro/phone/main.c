#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include "ssd1306x32.h"
#include "keypad4x4.h"
#include "serial_lib.h"
#include <util/delay.h>
#include <avr/interrupt.h>

void init_SIM800L(void);
void main_menu(void);
char menu(char* options[],char selection[]);
void procLine(void);
void dialNo(char *n);
void hangup(void);
void dialNoScrn(void);


int i = 0;
char toSIMstr[50];
char fromSIMstr[2][50];

int main(void){
    i2c_init();
    usart_init();
    lcd_init();
    keypad_init();
    _delay_ms(100);
    init_SIM800L();
    sei();

    lcd_clearFullDisplay();
    while (1){

        main_menu();
        //usart_recieveStr(fromSIMstr);
        //usart_recieveStr(toSIMstr);
        //strcat(fromSIMstr,"hello");
        //lcd_printStr(fromSIMstr,'t');
        _delay_ms(1000);
    }
    
}

void main_menu(void){
    char *mainMenu[] = {"Calls     ","Text      ","Phone Book","Calender  ","Settings  ","Extras    ",""};
    char  mainMenuSel[] = {'c','t','b','d','s','e'};

    char *callsMenu[] = {"Enter No. ","Missed    ","Dialed    ","Answered",""};
    char  callsMenuSel[] = {'r','m','d','a'};

    char *textMenu[] = {"Write Text","Unread    ","Read      ",""};
    char  textMenuSel[] = {'w','u','r'};

    char result = ' ';


    start:
    lcd_clearFullDisplay();
    result = menu(mainMenu,mainMenuSel);
    switch (result){
        case 'c':
            lcd_clearFullDisplay();
            result = menu(callsMenu,callsMenuSel);
            switch (result){
                case 'r':
                    dialNoScrn();
                break;
                case 'm':

                break;
                case 'd':

                break;
                case 'a':

                break;
            }
            /*
            if(result == 'C') goto start;
            lcd_clearFullDisplay();
            lcd_printStr("selected:",'m');
            lcd_print8x8(result);
            if(result == 'r') dialNo("+353833090923");
            lcd_setCursor(0,1);
            lcd_printStr("Phone code...",'m');
            */
        break;
        case 't':
            lcd_clearFullDisplay();
            result = menu(textMenu,textMenuSel);
            if(result == 'C') goto start;
            lcd_clearFullDisplay();
            lcd_printStr("selected:",'m');
            lcd_print8x8(result);
            lcd_setCursor(0,1);
            lcd_printStr("SMS code...",'m');
        break;
        case 'b':
            usart_sendStr("ATI\r\n");
    }
}
void init_SIM800L(void){
    usart_sendStr("AT+CLCC=1\r\n");
    usart_sendStr("ATE0\r\n");

}
void procLine(void){
    int i;
    lcd_setCursor(0,4);
    lcd_printStr("                ",'m');
    lcd_setCursor(0,4);
    for(i= 0;i<16 && (fromSIMstr[1][i] != '\0');i++)
        lcd_print8x8(fromSIMstr[1][i]);
    //lcd_setCursor(0,5);
    //for(;fromSIMstr[i] != '\0';i++)
    //    lcd_print8x8(fromSIMstr[i]);
}

void dialNoScrn(void){
    int  i;
    char c=' ', num[11];
    lcd_clearFullDisplay();
    lcd_printStr("Enter No:",'t');
    lcd_setCursor(0,2);
    for(i=0;(c = keypad_read()) != 'D';i++){
        lcd_print12x16(c,'t');
        num[i] = c;
    }
    num[i]='\0';
    lcd_clearFullDisplay();
    dialNo(num);


    lcd_printStr("Calling...",'t');
    lcd_setCursor(0,2);
    lcd_printStr(num,'t');
    //_delay_ms(4000);
    if(keypad_read() == 'D')
        hangup();

}

void dialNo(char *n){
    char fn[25];
    strcpy(fn,"ATD");
    strcat(fn,n);
    strcat(fn,";\n\r");
    usart_sendStr(fn);

}
void hangup(void){
    usart_sendStr("ATH\r\n");
}

char menu(char* options[],char selection[]){
    int currentView = 0;
    int cursorPos   = 0;
    unsigned char pressed = ' ';
    //keypad_read(); //clear buffer

    for(int i=0;i< 4 && *options[currentView+i] != '\0';i++){
        lcd_setCursor(0,i*2);
        if(cursorPos == i) lcd_invertPrint(1);
            lcd_printStr(options[currentView+i],'t');
        if(cursorPos == i) lcd_invertPrint(0);
    }

    while((pressed = keypad_read()) != 'D'){

        if(pressed == 'A'){
            if(cursorPos != 0)
                cursorPos--;
            else if(currentView != 0)
                currentView--;
        }
        else if (pressed == 'B'){
            if(*options[cursorPos+1] != '\0'&& cursorPos != 3)
                cursorPos++;
            else if (*options[currentView+cursorPos+1] != '\0')
                currentView++;
        }
        else if (pressed == 'C'){
            return 'C';
        }

        for(int i=0;i< 4 && *options[currentView+i] != '\0';i++){
            lcd_setCursor(0,i*2);
            if(cursorPos == i) lcd_invertPrint(1);
                lcd_printStr(options[currentView+i],'t');
            if(cursorPos == i) lcd_invertPrint(0);
        }

    }
    return selection[cursorPos+currentView];
}

ISR(USART_RX_vect){
    //cli();
    //int i;
    //for(i=0;fromSIMstr[i] != '\0';i++)
    //    fromSIMstr[i] = 0;
    //fromSIMstr[i] = 0;
    // <<<<<< use this to clear interupts until \0 from string to flag a re-enable
    usart_recieveStr(fromSIMstr);
    //procLine();
    //sei();
    //hello earth
}