#ifndef     SSD1306X32_H
#define     SSD1306X32_H
    #include    "i2c.h"
    #define LCD_ADDRESS 0X3C
    #define ON_INIT_CMD 0XA4
    #define ONN_CMD     0XAF
    #define OFF_CMD     0XAE


    #define COMMAND_REG 0X80   // bit 8 = 1 for command or 0 for data
    #define DATA_REG    0X40
    #define INVERT_CMD  0XA7
    #define NML_DSP_CMD 0XA6
    #define PGE_ADR_MD  0X02
    #define SCROLL_ON   0X2F
    #define SCROLL_OFF  0X2E

    #define SCREEN_SIZE 64  //or 32 (128x64 or 128x32)

    void lcd_init(void);
    void lcd_write_cmd(unsigned char cmd);
    void lcd_write_data(unsigned char data);
    void lcd_clear_data(void);
    void lcd_setCursor(unsigned char x, unsigned char y);
    void lcd_movCursor(unsigned char x, unsigned char y);
    void lcd_clearFullDisplay(void);
    void lcd_print12x16(unsigned char c,unsigned char f);
    void lcd_print8x8(unsigned char c);
    void lcd_printStr(char s[],unsigned char f);
    void lcd_printInt(int n,unsigned char f);
    void reverse_i2c(char s[]);
    void lcd_line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2);
    void lcd_square(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);



#endif