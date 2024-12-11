#ifndef KEYPAD4X4_H
#define KEYPAD4X4_H

#define KEY_PRT1     PORTD
#define KEY_DDR1     DDRD
#define KEY_PIN1     PIND

#define KEY_PRT2    PORTB
#define KEY_DDR2    DDRB
#define KEY_PIN2    PINB

void keypad_init(void);
char keypad_read(void);


#endif