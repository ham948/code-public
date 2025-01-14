/*___________________________________________________________________________________________________

Title:
	uart.h v2.2

Description:
	Library for USART on AVR devices
	
	For complete details visit:
	https://www.programming-electronics-diy.xyz/2022/08/uart-library-for-avr-microcontrollers.html

Author:
 	Liviu Istrate
	istrateliviu24@yahoo.com
	www.programming-electronics-diy.xyz

Donate:
	Software development takes time and effort so if you find this useful consider a small donation at:
	paypal.me/alientransducer
_____________________________________________________________________________________________________*/


/* ----------------------------- LICENSE - GNU GPL v3 -----------------------------------------------

* This license must be included in any redistribution.

* Copyright (c) 2022 Liviu Istrate, www.programming-electronics-diy.xyz (istrateliviu24@yahoo.com)

* Project URL: https://www.programming-electronics-diy.xyz/2022/08/uart-library-for-avr-microcontrollers.html

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.

--------------------------------- END OF LICENSE --------------------------------------------------*/


/* ----------------------------- CHANGELOG ------------------------------------------------------------
	v2.2
		31-01-2024:
		- included a function used to wait in a while loop until all bytes in the buffer have been transmitted.

	v2.1
		- UART1 can now be excluded by a preprocessor directive to save space
		
	v2.0
		- added support for modern AVR (UPDI devices). Tested on tinyAVR402.
		- all USART peripherals can now be controlled by a single library by the use of structures.
		- removed functions dedicated to 9-bit UART. Now 9-bit is supported by interrupts.
		
	v1.3
		- fixed the baudrate register that affected higher baud rates
		- variable "uartBytesToRead" is now also incremented inside ATOMIC_BLOCK(ATOMIC_RESTORESTATE)

	v1.2
		- variable "uartBytesToSend" is now incremented inside ATOMIC_BLOCK(ATOMIC_RESTORESTATE).
		  This was previously causing intermittent data scrambling.

-----------------------------------------------------------------------------------------------------*/


#ifndef UART_H_
#define UART_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdbool.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <string.h>
#include "utils.h"

#ifdef UCSR0A
	#define AVR_CLASS			0 // Classic AVR
#else
	#define AVR_CLASS			1 // Modern AVR (UPDI devices)
#endif

//-----------------------------------------------------------------------------
// USER SETTINGS
//-----------------------------------------------------------------------------
#define UART_USE_9BITS					0 // set to 1 to use 9-bit UART

// Set to 0 if the device has UART1 but is not used, to reduce code size
#define UART_USE_UART1					1

// When using 9-bit mode, buffer size must be 32 bytes
#define UART_TX_BUFFER_SIZE				32 // bytes from 1 to 255
#define UART_RX_BUFFER_SIZE				32 // bytes from 1 to 255

// When Synchronous mode is used, the XCK pin controls whether the transmission clock is input// (Slave mode) or output (Master mode). The corresponding port pin must be set to output for Master mode or to// input for Slave mode. The normal port operation of the XCK pin will be overridden.
#define UART_XCKn_DDR					DDRD
#define UART_XCKn_PIN					PD4

// F_CPU is best to be defined inside project properties in Microchip Studio
// or a Makefile if custom Makefiles are used.
// See https://www.programming-electronics-diy.xyz/2024/01/defining-fcpu.html
//#define F_CPU							16000000UL

#ifndef F_CPU
	#error "F_CPU not defined!"
#endif

// Receiver Mode select
// USART_RXMODE_NORMAL_gc - Normal mode
// USART_RXMODE_CLK2X_gc - CLK2x mode
// USART_RXMODE_GENAUTO_gc - Generic auto baud mode
// USART_RXMODE_LINAUTO_gc - LIN constrained auto baud mode
#if AVR_CLASS == 1
//#define UART_RX_MODE					USART_RXMODE_CLK2X_gc
#endif

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
// Operating Modes
#define UART_ASYNC						16	// Asynchronous Normal mode
//#define UART_ASYNC_DOUBLE_SPEED		8	// Asynchronous Double Speed mode
#define UART_SYNC						2	// Synchronous Master/Slave mode

// Parity Mode
#define UART_NO_PARITY					0
#define UART_EVEN_PARITY				2
#define UART_ODD_PARITY					3

// Character Size
#define UART_5_BIT						0
#define UART_6_BIT						1
#define UART_7_BIT						2
#define UART_8_BIT						3
// 9-bit UART mode is typically used to implement a 'single master/multiple slaves'
// bus scheme using the UART. In this scheme, a 9th bit is added to each transmitted
// byte between the MSB of the data byte and the STOP bit. This 9th bit indicates
// whether the preceding 8-bits should be interpreted as 'address' or as 'data'.
#define UART_9_BIT						7 // High byte first

// Used by some short delay loops
#define CYCLES_PER_US					(F_CPU / 1000000.0) // CPU cycles per microsecond
#define UART_TX_TIMEOUT					(CYCLES_PER_US * 30000); // ~0.5s timeout
#define UART_RX_TIMEOUT					(CYCLES_PER_US * 30000); // ~0.5s timeout

//-----------------------------------------------------------------------------
// DATA TYPES
//-----------------------------------------------------------------------------
#if UART_USE_9BITS == 1
	typedef uint16_t char_size_t;
#else
	typedef uint8_t char_size_t;
#endif

typedef void (*voidFuncPtr)(uint8_t);

#if AVR_CLASS == 0
typedef struct{
	volatile uint8_t* UART_UCSRA;
	volatile uint8_t* UART_UCSRB;
	volatile uint8_t* UART_UCSRC;
	volatile uint8_t* UART_UBRR_LOW;
	volatile uint8_t* UART_UBRR_HIGH;
	volatile uint8_t* UART_UDR;
	
	uint8_t UART_UCSRA_U2X;
	uint8_t UART_UMSEL0;
	uint8_t UART_UPM0;
	uint8_t UART_UCSZ0;
	uint8_t UART_UCSZ1;
	uint8_t UART_UCSZ2;
	uint8_t UART_UCSRB_RXEN;
	uint8_t UART_UCSRB_TXEN;
	uint8_t UART_UCSRB_UDRIE;
	uint8_t UART_RXCIE;
	uint8_t UART_UDRE;
	uint8_t UART_TXB8;
	uint8_t UART_RXC;
	uint8_t UART_TXC;
	uint8_t UART_MPCM;
} USART_t;
#endif


typedef struct{
	USART_t* uart_reg;
	voidFuncPtr uartRXfunc;
	
	#if UART_USE_9BITS == 1
	uint32_t bitnineTX; // 32 bits for 32 bytes buffer array
	uint32_t bitnineRX; // 32 bits for 32 bytes buffer array
	#endif
	
	uint8_t UART_TX_BUFFER[UART_TX_BUFFER_SIZE];
	uint8_t UART_RX_BUFFER[UART_RX_BUFFER_SIZE];

	uint8_t uartBytesToSend;
	uint8_t uartTXWriteIdx;
	uint8_t uartTXReadIdx;

	uint8_t uartBytesToRead;
	uint8_t uartRXWriteIdx;
	uint8_t uartRXReadIdx;
	
	uint8_t RX_ERROR_FLAGS;
} UARTstruct_t;

extern UARTstruct_t uart0;
extern UARTstruct_t uart1;

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void UART_begin(UARTstruct_t* uart, float baudrate, uint8_t mode, uint8_t parity, uint8_t bits);
uint8_t UART_send(UARTstruct_t* uart, char_size_t data);
uint8_t UART_sendString(UARTstruct_t* uart, char* s);
uint8_t UART_sendBytes(UARTstruct_t* uart, char_size_t* buff, uint8_t length);
void UART_mpcmEnable(UARTstruct_t* uart);
void UART_mpcmDisable(UARTstruct_t* uart);
void UART_mpcmSelectDevice(UARTstruct_t* uart, uint8_t address);

#ifdef UTILS_H_
void UART_sendInt(UARTstruct_t* uart, INT_SIZE number);
void UART_sendFloat(UARTstruct_t* uart, float number, uint8_t decimals);
#endif

void UART_sendHex8(UARTstruct_t* uart, uint8_t value);
void UART_sendHex16(UARTstruct_t* uart, uint16_t value);
void UART_sendHex32(UARTstruct_t* uart, uint32_t value);
bool UART_available(UARTstruct_t* uart);
void UART_isSending(UARTstruct_t* uart, float fcpu, float baudrate);
uint8_t UART_isError(UARTstruct_t* uart);
bool UART_frameError(UARTstruct_t* uart);
bool UART_bufferOverflowError(UARTstruct_t* uart);
bool UART_parityError(UARTstruct_t* uart);
void UART_clearErrorFlags(UARTstruct_t* uart);
char_size_t UART_read(UARTstruct_t* uart);
uint8_t UART_readBytes(UARTstruct_t* uart, char_size_t* buff, uint8_t length);
uint8_t UART_readBytesUntil(UARTstruct_t* uart, char character, char_size_t* buff, uint8_t length);
void UART_flush(UARTstruct_t* uart);
void UART_end(UARTstruct_t* uart);
void UART_setRXhandler(UARTstruct_t* uart, void (*rx_func)(uint8_t c));

#if AVR_CLASS == 1
void UART_enableStartFrameDetection(UARTstruct_t* uart);
void UART_enableOneWireMode(UARTstruct_t* uart);
#endif

#endif /* UART_H_ */