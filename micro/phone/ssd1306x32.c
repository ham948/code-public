#include "ssd1306x32.h"
#include "Rubik12x16font.h"
#include "minimal8x8font.h"
#include "Terminal_12x16_font.h"
//#include "serialtest.c"

unsigned char x_pos = 0;
unsigned char y_pos = 0;
unsigned char invert = 0;
unsigned char display_data [128][SCREEN_SIZE/8];


void lcd_init(void){
    // for 128x64 or 128x32 see 
    for(unsigned char j = 0;j<(SCREEN_SIZE/8);j++)
        for(unsigned char i = 0;i<128;i++)
            display_data[i][j] = 0X01;

    lcd_write_cmd(OFF_CMD);

    lcd_write_cmd(0XA8); //set mux ratio
    lcd_write_cmd(0X3F);   //mux ratio 1F for 128x32 or 3F for 128x64

    lcd_write_cmd(0X20); //set page address mode
    lcd_write_cmd(PGE_ADR_MD);
    //lcd_write_cmd(0X22);
    //lcd_write_cmd(0X00);
    //lcd_write_cmd(0X07);

    //lcd_write_cmd(0X00); //set lower nibble of column start address to 0 : 0B0000xxxx
    //lcd_write_cmd(0X10); // set higher nibble of column start point

    lcd_write_cmd(0X40); //set RAM start point to 0B01xxxxxx
    lcd_write_cmd(0XD3); //display offset
    lcd_write_cmd(0X00); //                       0B00xxxxxx
    lcd_write_cmd(0XA1); //set segment remap    last bit = 1 prints from left to right, 0 otherwise
    lcd_write_cmd(0XC8); //set scan direction
    lcd_write_cmd(0XDA); //set COM pint hardweare config (see datasheet, or PCB)
    lcd_write_cmd(0X12); //0B00xx0010  default 0B00010010    128x32: 0X02 for 128x64: 0X12
    lcd_write_cmd(0XB0); //enter GDDRAM page start address 0B10110xxx here its 0B10110000

    lcd_write_cmd(0X81); // set contrast from 1-255 (default on reset 7F=127 )
    lcd_write_cmd(0X80);
    
    lcd_write_cmd(ON_INIT_CMD); //init on screen
    lcd_write_cmd(NML_DSP_CMD); //not inverted

    
    lcd_write_cmd(0XD5); //set clock freq
    lcd_write_cmd(0X80); // some freq
    lcd_write_cmd(0XD9); // set pre-charge time, higher value means less blinking
    lcd_write_cmd(0XC2); // some time


    lcd_write_cmd(0XDB); //set vCom value, to 0.77Volts (see data sheet)
    lcd_write_cmd(0X20);
    lcd_write_cmd(0X8D); //enable charge pump during display on !!!!!!!!!!very important
    lcd_write_cmd(0X14);

    lcd_write_cmd(SCROLL_OFF); //deactivate scrolling
    lcd_write_cmd(ONN_CMD); //display on
    
    
    
    lcd_write_cmd(0X27); //set up horizontal scroll bit0 = 0(right scroll) or 1 (left scroll) dft:0X26
    lcd_write_cmd(0x00); // null byte
    lcd_write_cmd(0X00); // 0B00000xxx    xxx selects start page address
    lcd_write_cmd(0x00); // 0B00000xxx    xxx selects speed in frames per freq. (see data sheet)
    lcd_write_cmd(0x03); // 0B00000xxx    xxx selects end page address
    lcd_write_cmd(0X00); // null byte
    lcd_write_cmd(0XFF); //!null byte 

  //lcd_write_cmd(0X2F); //  activate scroll
  //lcd_write_cmd(0X2E); //deactivate scroll */
}

void lcd_write_cmd(unsigned char cmd){
    i2c_start();
    i2c_write(LCD_ADDRESS<<1); //bit 0 is 0 for write, 1 for read
    i2c_write(COMMAND_REG);
    i2c_write(cmd);
    i2c_stop();
}

void lcd_write_data(unsigned char data){  //<<<<<<<<<<<<<<maybe write a new function for invert

    if(display_data[x_pos][y_pos] != data){
    display_data[x_pos][y_pos] = data;

    lcd_write_cmd(0X00 + (x_pos & 0X0F));       //set column lower address
    lcd_write_cmd(0x10 + ((x_pos>>4) & 0X0F));  //set column highr address
    lcd_write_cmd(0XB0 + y_pos);   

    i2c_start();
    i2c_write(LCD_ADDRESS<<1); //bit 0 is 0 for write, 1 for read
    i2c_write(DATA_REG);
    i2c_write(display_data[x_pos][y_pos]);
    i2c_stop();
    }
}

void lcd_write_data_overlap(unsigned char data){
    if(display_data[x_pos][y_pos] != data){
    display_data[x_pos][y_pos] |= data;

    lcd_write_cmd(0X00 + (x_pos & 0X0F));       //set column lower address
    lcd_write_cmd(0x10 + ((x_pos>>4) & 0X0F));  //set column highr address
    lcd_write_cmd(0XB0 + y_pos);    

    i2c_start();
    i2c_write(LCD_ADDRESS<<1); //bit 0 is 0 for write, 1 for read
    i2c_write(DATA_REG);
    i2c_write(display_data[x_pos][y_pos]);
    i2c_stop();
    }
}

void lcd_clear_data(void){
    if(display_data[x_pos][y_pos] != 0X00){
    display_data[x_pos][y_pos] = 0;

    lcd_write_cmd(0X00 + (x_pos & 0X0F));       //set column lower address
    lcd_write_cmd(0x10 + ((x_pos>>4) & 0X0F));  //set column highr address
    lcd_write_cmd(0XB0 + y_pos);  

    i2c_start();
    i2c_write(LCD_ADDRESS<<1); //bit 0 is 0 for write, 1 for read
    i2c_write(DATA_REG);
    i2c_write(0X00);
    i2c_stop();
    }
    if(x_pos == 127) x_pos = 0;
    else x_pos +=1;
}

void lcd_invertPrint(unsigned char i){
    if(i == 1)
        invert = 0XFF;
    if(i == 0)
        invert = 0X00;
}

void lcd_setCursor(unsigned char x, unsigned char y){
    /*
    lcd_write_cmd(0X00 + (x & 0X0F));       //set column lower address
    lcd_write_cmd(0x10 + ((x>>4) & 0X0F));  //set column highr address
    lcd_write_cmd(0XB0 + y);                //set page address
    */
    // x is column number y is page number
    x_pos = x;
    y_pos = y;
}

void lcd_movCursor(unsigned char x,unsigned char y){
    lcd_setCursor(x_pos+x,y_pos+y);
}


void lcd_clearFullDisplay(void){
    lcd_setCursor(0,0);
    for(unsigned char page = 0; page < (SCREEN_SIZE/8) ; page++){     //4 pages for 128x32 8 for 128x64
        lcd_setCursor(0,page);
        for(unsigned char colm = 0;colm < 128;colm++){
            lcd_write_data(0XFF);
        }
    }
    lcd_setCursor(0,0);
        for(unsigned char page = 0; page < (SCREEN_SIZE/8) ; page++){
        lcd_setCursor(0,page);
        for(unsigned char colm = 0;colm < 128;colm++){
            lcd_clear_data();
        }
    }
    for(unsigned char j = 0;j<(SCREEN_SIZE/8);j++)
        for(unsigned char i = 0;i<128;i++)
            display_data[i][j] = 0X00;
    lcd_setCursor(0,0);
}

void lcd_print12x16(unsigned char c, unsigned char f){
    unsigned char i, j;
    c -= 32;
    /*
    for(unsigned char k = 0;k<12;k++) // clears old characters
        for(unsigned char l = 0;l<2;l++){
            lcd_clear_data();
            if(l == 1) lcd_movCursor(1,-1);
            else lcd_movCursor(0,+1);
        }
    lcd_movCursor(-12,0);
    */
    for(i = 0;i<12;i++)
        for(j = 0; j<2; j++){
            if(f == 'r'){
                //lcd_clear_data();
                //lcd_movCursor(-1,0);
                lcd_write_data(pgm_read_byte(&Rubik12x16[1+c*25+i*2+j]));
            }
            if(f == 't'){
                //lcd_clear_data();
                //lcd_movCursor(-1,0);  //<<<<<<<<<<<these 2 lines are the problem
                lcd_write_data(invert^pgm_read_byte(&Terminal12x16[1+c*25+i*2+j]));
            }
            if(j == 1) lcd_movCursor(1,-1);
            else lcd_movCursor(0,+1);
        }
    //lcd_movCursor(12,16); 
}

void lcd_print8x8(unsigned char c){
    unsigned char i;
    c -= 32;
    for(i = 0;i<8;i++){
        lcd_clear_data();
        lcd_movCursor(-1,0);
        lcd_write_data(pgm_read_byte(&MS_UI_Gothic8x8[1+c*9+i]));
        lcd_movCursor(1,0);
    }
    //lcd_movCursor(12,16);
    
}

void lcd_printStr(char s[],unsigned char f){
    if(f == 'r')
        for(unsigned char i = 0;s[i] != '\0';i++)
            lcd_print12x16(s[i],'r');
    if(f == 't')
        for(unsigned char i = 0;s[i] != '\0';i++)
            lcd_print12x16(s[i],'t');
    if(f == 'm')
        for(unsigned char i = 0;s[i] != '\0';i++)
            lcd_print8x8(s[i]);
}

void lcd_printInt(int n,unsigned char f){
  char out[] = "+00000";
  unsigned char i = 0, sign = 1;
  if(n < 0){
    sign = 0;
    n = -n;
  }
  do{
    out[i++] = n % 10 + '0';
  }while((n/=10)>0);
  while(i<5)
    out[i++] = '0';
  out[i++] = (sign)? '+' : '-';
  reverse_i2c(out);
  lcd_printStr(out,f);
}

void reverse_i2c(char s[]){
    int c, i, j;

    for(i = 0, j = 5; i < j; i++, j--){
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void lcd_line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2){
    unsigned char lit_bit1,lit_bit2,lit_row1,lit_row2;
    lit_bit1 = y1 % 8;
    lit_bit2 = y2 % 8;
    lit_row1 = y1 / 8;
    lit_row2 = y2 / 8;

    if(y1 == y2){
        for(unsigned char i = x1;i<=x2;i++){
            lcd_setCursor(i,lit_row1);
            lcd_write_data_overlap(1<<lit_bit1);
        }
    }else if(x1 == x2){
        lcd_setCursor(x1,lit_row1);
        lcd_write_data_overlap(0XFF<<lit_bit1);
        for(unsigned char i = lit_row1+1;i<lit_row2;i++){
            lcd_setCursor(x1,i);
            lcd_write_data_overlap(0XFF);
        }
        lcd_setCursor(x1,lit_row2);
        lcd_write_data_overlap(0XFF>>(7-lit_bit2));
    }
    
}

void lcd_square(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2){
    lcd_line(x1,y1,x2,y1); //horizontal lines
    lcd_line(x1,y2,x2,y2);

    lcd_line(x1,y1,x1,y2);
    lcd_line(x2,y1,x2,y2);
}
