MCU=atmega328p                                                             
F_CPU=16000000L
PORT=/dev/ttyACM0
BAUD=115200
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-std=gnu99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} #was -std=c99 but changes for asm() func
TARGET=main
SRCS=main.c sd.c uart.c fat.c utils.c
INCS=include

all:
	${CC} ${CFLAGS} -o ${TARGET}.bin -I ${INCS} ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

flash:
	avrdude -c arduino -P ${PORT} -b ${BAUD} -p m328p -U flash:w:${TARGET}.hex:i

clean:
	rm -f *.bin *.hex
