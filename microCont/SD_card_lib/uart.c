/*___________________________________________________________________________________________________

Title:
	uart.c v2.2

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


#include "uart.h"

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
#if AVR_CLASS == 0
USART_t USART0 = {
	.UART_UCSRA = &UCSR0A,
	.UART_UCSRB = &UCSR0B,
	.UART_UCSRC = &UCSR0C,
	.UART_UBRR_LOW = &UBRR0L,
	.UART_UBRR_HIGH = &UBRR0H,
	.UART_UDR = &UDR0,
	
	.UART_UCSRA_U2X = U2X0,
	.UART_UMSEL0 = UMSEL00,
	.UART_UPM0 = UPM00,
	.UART_UCSZ0 = UCSZ00,
	.UART_UCSZ1 = UCSZ01,
	.UART_UCSZ2 = UCSZ02,
	.UART_UCSRB_RXEN = RXEN0,
	.UART_UCSRB_TXEN = TXEN0,
	.UART_UCSRB_UDRIE = UDRIE0,
	.UART_RXCIE = RXCIE0,
	.UART_UDRE = UDRE0,
	.UART_TXB8 = TXB80,
	.UART_RXC = RXC0,
	.UART_TXC = TXC0,
	.UART_MPCM = MPCM0,
};

#ifdef UCSR1A
USART_t USART1 = {
	.UART_UCSRA = &UCSR1A,
	.UART_UCSRB = &UCSR1B,
	.UART_UCSRC = &UCSR1C,
	.UART_UBRR_LOW = &UBRR1L,
	.UART_UBRR_HIGH = &UBRR1H,
	.UART_UDR = &UDR1,
	
	.UART_UCSRA_U2X = U2X1,
	.UART_UMSEL0 = UMSEL10,
	.UART_UPM0 = UPM10,
	.UART_UCSZ0 = UCSZ10,
	.UART_UCSZ1 = UCSZ11,
	.UART_UCSZ2 = UCSZ12,
	.UART_UCSRB_RXEN = RXEN1,
	.UART_UCSRB_TXEN = TXEN1,
	.UART_UCSRB_UDRIE = UDRIE1,
	.UART_RXCIE = RXCIE1,
	.UART_UDRE = UDRE1,
	.UART_TXB8 = TXB81,
	.UART_RXC = RXC1,
	.UART_TXC = TXC1,
	.UART_MPCM = MPCM1,
};
#endif
#endif

UARTstruct_t uart0 = {
	.uart_reg = &USART0
};

#ifdef UCSR1A
#if UART_USE_UART1 == 1
UARTstruct_t uart1 = {
	.uart_reg = &USART1
};
#endif
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
	Initialization function sets the Baud rate, frame format, enables UART 
	interrupts and global interrupts. One STOP bit is used. The function
	can be used at any point to change baud rate or frame parameters.

	baudrate	bits per second
	
	mode		UART mode: Asynchronous Normal mode, Asynchronous Double Speed,
				Synchronous Master/Slave mode.

				Available constants: 
					UART_ASYNC
					UART_SYNC
					
	parity		UART_NO_PARITY, UART_EVEN_PARITY, UART_ODD_PARITY
	
	bits		number of bits in the frame, from 5 to 8.
				UART_5_BIT, UART_6_BIT, UART_7_BIT, UART_8_BIT, UART_9_BIT.
------------------------------------------------------------------------------*/
void UART_begin(UARTstruct_t* uart, float baudrate, uint8_t mode, uint8_t parity, uint8_t bits){
	
	// Reset to 0 in case there are communication issues 
	// and the function is called again
	uart->RX_ERROR_FLAGS = 0;
	uart->uartBytesToSend = 0;
	uart->uartTXWriteIdx = 0;
	uart->uartTXReadIdx = 0;
	uart->uartBytesToRead = 0;
	uart->uartRXWriteIdx = 0;
	uart->uartRXReadIdx = 0;
	
	// For interrupt driven USART operation, the Global Interrupt Flag should
	// be cleared (and interrupts globally disabled) when doing the initialization.
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){		
		// Note for external clock: in Slave mode the fxck must be < than fosc / 4
		// Baud Rate Register Setting
		uint16_t ubrr;
		
		// Reset UART Registers
		#if AVR_CLASS == 0
			*uart->uart_reg->UART_UCSRA = 0;
			*uart->uart_reg->UART_UCSRB = 0;
			*uart->uart_reg->UART_UCSRC = 0;
		#else
			uart->uart_reg->CTRLB = 0;
			uart->uart_reg->CTRLC = 0;
		#endif
		
		// Asynchronous USART
		if(mode == UART_ASYNC){
			if(baudrate < 57600){
				// Normal Mode
				#if AVR_CLASS == 0
					ubrr = ((F_CPU / (8L * baudrate)) - 1) / 2;
				#else
					ubrr = (float)(F_CPU * 64 / (16 * baudrate)) + 0.5;
				#endif
			}else{
				// Double Speed Operation
				// In this mode, the receiver will use half the number of samples (reduced from 16 to 8) for data
				// sampling and clock recovery. This requires a more accurate baud rate setting and peripheral clock.
				#if AVR_CLASS == 0
					ubrr = ((F_CPU / (4L * baudrate)) - 1) / 2;
					*uart->uart_reg->UART_UCSRA |= (1 << uart->uart_reg->UART_UCSRA_U2X); // baud doubler on for high baud rates, i.e. 115200
				#else
					ubrr = (float)(F_CPU * 64 / (8 * baudrate)) + 0.5;
					uart->uart_reg->CTRLB |= USART_RXMODE_CLK2X_gc;
				#endif
			}
		}else{
			// Synchronous USART (clock polarity is UCPOL 0)
			#if AVR_CLASS == 0
				*uart->uart_reg->UART_UCSRC |= (1 << uart->uart_reg->UART_UMSEL0);
				ubrr = ((F_CPU / (2L * baudrate)) - 1);
			#else
				ubrr = (float)(F_CPU / (2 * baudrate)) + 0.5;
				uart->uart_reg->CTRLC |= USART_CMODE_SYNCHRONOUS_gc;
			#endif
		}
		
		// Set baudrate register
		#if AVR_CLASS == 0
			*uart->uart_reg->UART_UBRR_HIGH = ubrr >> 8;
			*uart->uart_reg->UART_UBRR_LOW = ubrr;
		#else
			uart->uart_reg->BAUD = ubrr; // e.g: USART0.BAUD = ubrr
		#endif
		
		// Parity, 1 Stop bit (default)
		#if AVR_CLASS == 0
			*uart->uart_reg->UART_UCSRC |= (parity << uart->uart_reg->UART_UPM0);
		#else
			uart->uart_reg->CTRLC |= (parity << USART_PMODE_gp);
		#endif
		
		// Number of bits (Character Size)
		#if AVR_CLASS == 0
			*uart->uart_reg->UART_UCSRC |= ((bits << uart->uart_reg->UART_UCSZ0) & 0x06);
			
			if(bits == UART_9_BIT){
				*uart->uart_reg->UART_UCSRB |= (1 << 2);
			}
		#else
			uart->uart_reg->CTRLC |= bits;
		#endif
		
		// Enable transmitter and receiver
		#if AVR_CLASS == 0
			*uart->uart_reg->UART_UCSRB |= (1 << uart->uart_reg->UART_UCSRB_RXEN) | (1 << uart->uart_reg->UART_UCSRB_TXEN);
		#else
			uart->uart_reg->CTRLB |= (1 << USART_RXEN_bp) | (1 << USART_TXEN_bp);
		#endif
	}
	
	// RX Complete Interrupt Enable
	#if AVR_CLASS == 0
		*uart->uart_reg->UART_UCSRB |= (1 << uart->uart_reg->UART_RXCIE);
	#else
		uart->uart_reg->CTRLA |= (1 << USART_RXCIE_bp);
	#endif
		
	// Enable global interrupts
	sei();
}



/*-----------------------------------------------------------------------------
	Put a byte in the transmit buffer and enables the USART Data Register 
	Empty Interrupt. If the UART is somehow disabled, a timeout will end the
	while loop after 1s.
------------------------------------------------------------------------------*/
uint8_t UART_send(UARTstruct_t* uart, char_size_t data){
	uint32_t timeout = UART_TX_TIMEOUT;
	
	// Check for buffer overflow
	if(uart->uartTXWriteIdx == UART_TX_BUFFER_SIZE){
		uart->uartTXWriteIdx = 0;
		
		// Overflow condition means the write index is now behind the read index.
		// Wait until the next byte in the buffer is sent.
		// In case the UART is disabled the timeout will occur.
		while(uart->uartBytesToSend){
			if(timeout) timeout--;
			else return 1;
		}
	}
	
	// Put data byte into the transmit buffer
	uart->UART_TX_BUFFER[uart->uartTXWriteIdx] = data;
	
	// When character size is 9-bit
	#if UART_USE_9BITS == 1
		if(data & 0x100){
			uart->bitnineTX |= ((uint32_t)1 << uart->uartTXWriteIdx);
		}else{
			uart->bitnineTX &= ~((uint32_t)1 << uart->uartTXWriteIdx);
		}
	#endif
	
	uart->uartTXWriteIdx++;
	
	// This must be changed atomically otherwise intermittent 
	// errors will occur depending on other interrupts
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		uart->uartBytesToSend++;
	}
	
	// Enable USART Data Register Empty Interrupt
	#if AVR_CLASS == 0
		*uart->uart_reg->UART_UCSRB |= (1 << uart->uart_reg->UART_UCSRB_UDRIE);
	#else
		uart->uart_reg->CTRLA |= USART_DREIE_bm;
	#endif
	
	return 0;
}


/*-----------------------------------------------------------------------------
	Sends a null terminated string. Returns 1 if error.
------------------------------------------------------------------------------*/
uint8_t UART_sendString(UARTstruct_t* uart, char* s){
	// Transmit a character until NULL is reached
	while(*s){
		if(UART_send(uart, *s++)){
			// Exit if timeout
			return 1;
		}
	}
	
	return 0;
}


/*-----------------------------------------------------------------------------
	Send a series of bytes in a buffer. Returns 1 if error.
------------------------------------------------------------------------------*/
uint8_t UART_sendBytes(UARTstruct_t* uart, char_size_t* buff, uint8_t length){
	for(uint8_t i = 0; i < length; i++){
		if(UART_send(uart, buff[i])){
			// Exit if timeout
			return 1;
		}
	}
	
	return 0;
}


/*-----------------------------------------------------------------------------
	Enables the Multi-processor Communication mode (MPCM).
	After this function sets the MPCM bit, frames that do not contain an address
	will be ignored by the UART. Frames containing an address have the 
	9'th bit set to 1.
------------------------------------------------------------------------------*/
void UART_mpcmEnable(UARTstruct_t* uart){
	// This bit enables the Multi-processor Communication mode. 
	// When the MPCM bit is written to one, all the incoming frames 
	// received by the USART Receiver that do not contain address 
	// information will be ignored. The Transmitter is unaffected 
	// by the MPCM setting.
	#if AVR_CLASS == 0
		*uart->uart_reg->UART_UCSRA |= (1 << uart->uart_reg->UART_MPCM);
	#else
		uart->uart_reg->CTRLB |= (1 << USART_MPCM_bp);
	#endif
}


/*-----------------------------------------------------------------------------
	Disables the Multi-processor Communication mode (MPCM).
	After this function sets the MPCM bit to 0, data frames can be received.
	This should be used after a valid address has been received after using
	UART_mpcmEnable().
------------------------------------------------------------------------------*/
void UART_mpcmDisable(UARTstruct_t* uart){	
	// This bit enables the Multi-processor Communication mode.
	// When the MPCM bit is written to one, all the incoming frames
	// received by the USART Receiver that do not contain address
	// information will be ignored. The Transmitter is unaffected
	// by the MPCM setting.
	#if AVR_CLASS == 0
		*uart->uart_reg->UART_UCSRA &= ~(1 << uart->uart_reg->UART_MPCM);
	#else
		uart->uart_reg->CTRLB &= ~(1 << USART_MPCM_bp);
	#endif
}


/*-----------------------------------------------------------------------------
	Select a device by sending an address frame over UART.
------------------------------------------------------------------------------*/
void UART_mpcmSelectDevice(UARTstruct_t* uart, uint8_t address){
	// Set 9'th bit to 1 indicating an address frame
	uint16_t data = address | 0x0100;
	UART_send(uart, data);
}


/*-----------------------------------------------------------------------------
	Send an integer number.
------------------------------------------------------------------------------*/
#ifdef UTILS_H_
void UART_sendInt(UARTstruct_t* uart, INT_SIZE number){
	char string[MAX_NR_OF_DIGITS + 1] = {0};
	
	STRING_itoa(number, string, 0);
	UART_sendString(uart, string);
}


/*-----------------------------------------------------------------------------
	Send an float number.
	
	decimals	number of digits after the dot
------------------------------------------------------------------------------*/
void UART_sendFloat(UARTstruct_t* uart, float number, uint8_t decimals){
	char string_integer[MAX_NR_OF_DIGITS + 1];
	char string_float[MAX_NR_OF_DIGITS + 1];
	
	STRING_ftoa(number, string_integer, string_float, 0, decimals);

	if(number < 0) UART_send(uart, '-');
	UART_sendString(uart, string_integer);
	UART_send(uart, '.');
	UART_sendString(uart, string_float);
}
#endif


/*-----------------------------------------------------------------------------
	Convert 1-byte integer number into hex and send over UART.
------------------------------------------------------------------------------*/
void UART_sendHex8(UARTstruct_t* uart, uint8_t value){
	// Extract upper and lower nibbles from input value
	uint8_t upperNibble = (value & 0xF0) >> 4;
	uint8_t lowerNibble = value & 0x0F;

	// Convert nibble to its ASCII hex equivalent
	upperNibble += upperNibble > 9 ? 'A' - 10 : '0';
	lowerNibble += lowerNibble > 9 ? 'A' - 10 : '0';

	// Print the characters
	UART_send(uart, upperNibble);
	UART_send(uart, lowerNibble);
}


/*-----------------------------------------------------------------------------
	Convert 2-byte integer number into hex and send over UART.
------------------------------------------------------------------------------*/
void UART_sendHex16(UARTstruct_t* uart, uint16_t value){
	// Transmit upper 8 bits
	UART_sendHex8(uart, value >> 8);

	// transmit lower 8 bits
	UART_sendHex8(uart, (uint8_t) value);
}


/*-----------------------------------------------------------------------------
	Convert 4-byte integer number into hex and send over UART.
------------------------------------------------------------------------------*/
void UART_sendHex32(UARTstruct_t* uart, uint32_t value){
	// Transmit upper 16 bits
	UART_sendHex16(uart, (uint16_t) (value >> 16));

	// transmit lower 16 bits
	UART_sendHex16(uart, (uint16_t) value);
}


/*-----------------------------------------------------------------------------
	Returns true if new data in available.
------------------------------------------------------------------------------*/
bool UART_available(UARTstruct_t* uart){
	return (uart->uartBytesToRead != 0);
}


/*-----------------------------------------------------------------------------
	Waits in a while loop until all bytes in the buffer have been transmitted.
	Can be used before putting the microcontroller to sleep to ensure
	all data has been transmitted.
------------------------------------------------------------------------------*/
void UART_isSending(UARTstruct_t* uart, float fcpu, float baudrate){
	// Time it takes to send 1 byte or 8 bits in us
	uint16_t byte_time_us = 8.0 / baudrate * 1000000;
	
	// CPU cycles in a us * nr of us for 8 bits
	uint16_t cycles_delay = fcpu / 1000000.0 * byte_time_us;
	
	while(uart->uartBytesToSend){};

	// Delay x number of CPU cycles because even if the TX interrupt is disabled
	// there is one more byte in the TX buffer currently being transmitted.
	for(;cycles_delay > 0; cycles_delay--){
		asm("nop");
	}
}


/*-----------------------------------------------------------------------------
	Returns error flags. Use provided functions to check for individual flags.
------------------------------------------------------------------------------*/
uint8_t UART_isError(UARTstruct_t* uart){
	uint8_t flags = 0;
	
	#if AVR_CLASS == 0
		flags = (uart->RX_ERROR_FLAGS & 0x1C); // Keep only error flags
	#else
		flags = (uart->RX_ERROR_FLAGS & 0x46); // Keep only error flags
	#endif
	
	return flags;
}


bool UART_frameError(UARTstruct_t* uart){
	#if AVR_CLASS == 0
		return uart->RX_ERROR_FLAGS & (1 << 4);
	#else
		return uart->RX_ERROR_FLAGS & (1 << 2);
	#endif
}


bool UART_bufferOverflowError(UARTstruct_t* uart){
	#if AVR_CLASS == 0
		return uart->RX_ERROR_FLAGS & (1 << 3);
	#else
		return uart->RX_ERROR_FLAGS & (1 << 6);
	#endif
}


bool UART_parityError(UARTstruct_t* uart){
	#if AVR_CLASS == 0
		return uart->RX_ERROR_FLAGS & (1 << 2);
	#else
		return uart->RX_ERROR_FLAGS & (1 << 1);
	#endif
}


void UART_clearErrorFlags(UARTstruct_t* uart){
	uart->RX_ERROR_FLAGS = 0;
}


/*-----------------------------------------------------------------------------
	Returns the next received byte or 0 if no new data is available.
	Should be used only if UART_available()	returns true.
------------------------------------------------------------------------------*/
char_size_t UART_read(UARTstruct_t* uart){
	char_size_t rx = 0;
	
	if(uart->uartBytesToRead){
		// Check for buffer overflow
		if(uart->uartRXReadIdx == UART_RX_BUFFER_SIZE){
			uart->uartRXReadIdx = 0;
		}
		
		rx = uart->UART_RX_BUFFER[uart->uartRXReadIdx];
		
		#if UART_USE_9BITS == 1
			if(uart->bitnineRX & ((uint32_t)1 << uart->uartRXReadIdx)){
				rx |= (1 << 8);
			}
		#endif
		
		uart->uartRXReadIdx++;
		
		// This must be changed atomically otherwise intermittent
		// errors will occur depending on other interrupts
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			uart->uartBytesToRead--;
		}
	}
	
	return rx;
}


/*-----------------------------------------------------------------------------
	Reads received bytes into the provided buffer. The function terminates 
	if the determined length has been read, or it times out.
	
	Returns the number of characters read.
------------------------------------------------------------------------------*/
uint8_t UART_readBytes(UARTstruct_t* uart, char_size_t* buff, uint8_t length){
	uint32_t timeout = UART_RX_TIMEOUT;
	
	for(uint8_t i = 0; i < length; i++){
		while(UART_available(uart) == false){
			if(timeout) timeout--;
			else return i;
		}
		
		buff[i] = UART_read(uart);
	}
	
	return length;
}


/*-----------------------------------------------------------------------------
	Reads received bytes into the provided buffer. The function terminates 
	if the determined length has been read, if it times out or if the 
	terminator character is detected. The termination character is not 
	included into the buffer.
	
	Returns the number of characters read.
------------------------------------------------------------------------------*/
uint8_t UART_readBytesUntil(UARTstruct_t* uart, char character, char_size_t* buff, uint8_t length){
	uint32_t timeout = UART_RX_TIMEOUT;
	
	for(uint8_t i = 0; i < length; i++){
		while(UART_available(uart) == false){
			if(timeout) timeout--;
			else return 0;
		}
		
		buff[i] = UART_read(uart);
		if(buff[i] == character){
			buff[i] = 0;
			return i;
		}
	}
	
	return 0;
}


/*-----------------------------------------------------------------------------
	If the buffer has to be flushed during normal operation, due to 
	for instance an error condition, read the UDRn I/O location until 
	the RXCn Flag is cleared.
------------------------------------------------------------------------------*/
void UART_flush(UARTstruct_t* uart){
	volatile uint8_t dummy;
	
	#if AVR_CLASS == 0
		while(*uart->uart_reg->UART_UCSRA & (1 << uart->uart_reg->UART_RXC)){
			dummy = *uart->uart_reg->UART_UDR; // discard received data
			(void)dummy;
		}
	#else
		while(uart->uart_reg->STATUS & (1 << USART_RXCIF_bp)){
			dummy = uart->uart_reg->RXDATAL; // discard received data
			(void)dummy;
		}
	#endif
}


/*-----------------------------------------------------------------------------
	Disable UART.
	When the transmitter is disabled, it will no longer override the TxDn pin, 
	and the pin direction is set as input automatically by hardware, 
	even if it was configured as output by the user.
------------------------------------------------------------------------------*/
void UART_end(UARTstruct_t* uart){
	// Disable the transmitter and receiver
	// Disable RX Complete Interrupt
	#if AVR_CLASS == 0
		*uart->uart_reg->UART_UCSRB = 0;
	#else
		uart->uart_reg->CTRLA = 0;
		uart->uart_reg->CTRLB = 0;
	#endif
}


/*-----------------------------------------------------------------------------
	Redirects received data to a user function.
------------------------------------------------------------------------------*/
void UART_setRXhandler(UARTstruct_t* uart, void (*rx_func)(uint8_t c)){
	// set the receive interrupt to run the supplied user function
	uart->uartRXfunc = rx_func;
}


/*-----------------------------------------------------------------------------
	The Start Frame detector is able to	wake-up the system from 
	Idle or Standby Sleep modes when a high (IDLE) to low (START) transition is
	detected on the RxD line.
------------------------------------------------------------------------------*/
#if AVR_CLASS == 1
void UART_enableStartFrameDetection(UARTstruct_t* uart){
	uart->uart_reg->CTRLB |= (1 << 4);
}


/*-----------------------------------------------------------------------------
	In this mode, the TxD pin is connected to the RxD pin internally. 
	If the receiver is enabled when	transmitting it will receive what 
	the transmitter is sending. This can be used to check that no one else is
	trying to transmit since received data will not be the same as the transmitted data.
------------------------------------------------------------------------------*/
void UART_enableOneWireMode(UARTstruct_t* uart){
	// Enable internal pull-up
	// This must be done by the user since the UART TX pin can differ.
	//PORTA.PIN6CTRL |= PORT_PULLUPEN_bm;
	
	// Enable loop-back mode.
	// Enables an internal connection between the TxD and RxD pin.
	uart->uart_reg->CTRLA |= USART_LBME_bm;
	
	// Enable Open-drain mode.
	// Writing this bit to '1' makes the TxD pin to have open-drain functionality.
	// A pull-up resistor is needed to prevent the line from floating 
	// when a logic one is output to the TxD pin.
	uart->uart_reg->CTRLB |= USART_ODME_bm;
}
#endif



//-----------------------------------------------------------------------------
// ISR Handlers
//-----------------------------------------------------------------------------

/* TX */
// USART0 Data Register Empty Interrupt
#if AVR_CLASS == 0
	ISR(USART_UDRE_vect){	
#else
	ISR(USART_DRE_vect){
#endif
	// Check for buffer overflow
	if(uart0.uartTXReadIdx == UART_TX_BUFFER_SIZE){
		uart0.uartTXReadIdx = 0;
	}
	
	// Put data into transmit buffer
	#if AVR_CLASS == 0
		#if UART_USE_9BITS == 1
			// Write 9th bit first
			if(uart0.bitnineTX & ((uint32_t)1 << uart0.uartTXReadIdx)){
				UCSR0B |= (1 << TXB80);
			}else{
				UCSR0B &= ~(1 << TXB80);
			}
		#endif
		
		UDR0 = uart0.UART_TX_BUFFER[uart0.uartTXReadIdx++];
	#else
		#if UART_USE_9BITS == 1
			if(uart0.bitnineTX & ((uint32_t)1 << uart0.uartTXReadIdx)){
				USART0.TXDATAH |= 0x01; // high byte first
			}else{
				USART0.TXDATAH &= ~0x01; // high byte first
			}
		#endif

		USART0.TXDATAL = uart0.UART_TX_BUFFER[uart0.uartTXReadIdx++];
	#endif
		
	uart0.uartBytesToSend--;
	
	if(uart0.uartBytesToSend == 0){
		// Disable USART Data Register Empty Interrupt
		#if AVR_CLASS == 0
			UCSR0B &= ~(1 << UDRIE0);
		#else
			USART0.CTRLA &= ~(USART_DREIE_bm);
		#endif
	}	
}

/* TX */
// USART1 Data Register Empty Interrupt
#if UART_USE_UART1 == 1
#if defined(USART1_UDRE_vect) || defined(USART1_DRE_vect)
#if AVR_CLASS == 0
	ISR(USART1_UDRE_vect){
#else
	ISR(USART1_DRE_vect){
#endif
	// Check for buffer overflow
	if(uart1.uartTXReadIdx == UART_TX_BUFFER_SIZE){
		uart1.uartTXReadIdx = 0;
	}
	
	// Put data into transmit buffer
	#if AVR_CLASS == 0
		#if UART_USE_9BITS == 1
			// Write 9th bit first
			if(uart1.bitnineTX & ((uint32_t)1 << uart1.uartTXReadIdx)){
				UCSR1B |= (1 << TXB81);
			}else{
				UCSR1B &= ~(1 << TXB81);
			}
		#endif
		
		UDR1 = uart1.UART_TX_BUFFER[uart1.uartTXReadIdx++];
	#else
		#if UART_USE_9BITS == 1
			if(uart1.bitnineTX & ((uint32_t)1 << uart1.uartTXReadIdx)){
				USART1.TXDATAH |= 0x01; // high byte first
			}else{
				USART1.TXDATAH &= ~0x01; // high byte first
			}
		#endif
	
		USART1.TXDATAL = uart1.UART_TX_BUFFER[uart1.uartTXReadIdx++];
	#endif
	
	uart1.uartBytesToSend--;
	
	if(uart1.uartBytesToSend == 0){
		// Disable USART Data Register Empty Interrupt
		#if AVR_CLASS == 0
			UCSR1B &= ~(1 << UDRIE1);
		#else
			USART1.CTRLA &= ~(USART_DREIE_bm);
		#endif
	}
}
#endif
#endif



/* RX */
// USART0 Receive Complete Interrupt
#if AVR_CLASS == 0
	ISR(USART_RX_vect){
#else
	ISR(USART_RXC_vect){
#endif
	// Check for buffer overflow
	if(uart0.uartRXWriteIdx == UART_RX_BUFFER_SIZE){
		uart0.uartRXWriteIdx = 0;
	}
	
	// Due to the buffering of the Error Flags,
	// the UCSRnA must be read before the receive buffer (UDRn).
	// OR is used to preserve previous set flags.
	#if AVR_CLASS == 0
		uart0.RX_ERROR_FLAGS |= UCSR0A;
	#else
		uart0.RX_ERROR_FLAGS |= USART0.RXDATAH;
	#endif
	
	// Put data into receive buffer
	#if AVR_CLASS == 0
		#if UART_USE_9BITS == 1
			// Get status and 9th bit, then data from buffer
			// Must be read before reading the low bits from UDRn
			if(UCSR0B & (1 << RXB80)){
				uart0.bitnineRX |= ((uint32_t)1 << uart0.uartRXWriteIdx);
			}else{
				uart0.bitnineRX &= ~((uint32_t)1 << uart0.uartRXWriteIdx);
			}
		#endif
	
		uart0.UART_RX_BUFFER[uart0.uartRXWriteIdx] = UDR0;
	#else
	
		#if UART_USE_9BITS == 1
			if(USART0.RXDATAH & 0x01){ // Read high byte first
				uart0.bitnineRX |= ((uint32_t)1 << uart0.uartRXWriteIdx);
			}else{
				uart0.bitnineRX &= ~((uint32_t)1 << uart0.uartRXWriteIdx);
			}
		#endif
	
		uart0.UART_RX_BUFFER[uart0.uartRXWriteIdx] = USART0.RXDATAL;
	#endif
	
	// If there's a user function to handle this receive event
	if(uart0.uartRXfunc){
		// Call it and pass the received data
		uart0.uartRXfunc(uart0.UART_RX_BUFFER[uart0.uartRXWriteIdx]);
	}
	
	uart0.uartRXWriteIdx++;
	uart0.uartBytesToRead++;
}


/* RX */
// USART1 Receive Complete Interrupt
#if UART_USE_UART1 == 1
#if defined(USART1_RX_vect) || defined(USART1_RXC_vect)
#if AVR_CLASS == 0
	ISR(USART1_RX_vect){
#else
	ISR(USART1_RXC_vect){
#endif
	// Check for buffer overflow
	if(uart1.uartRXWriteIdx == UART_RX_BUFFER_SIZE){
		uart1.uartRXWriteIdx = 0;
	}
		
	// Due to the buffering of the Error Flags,
	// the UCSRnA must be read before the receive buffer (UDRn).
	// OR is used to preserve previous set flags.
	#if AVR_CLASS == 0
		uart1.RX_ERROR_FLAGS |= UCSR1A;
	#else
		uart1.RX_ERROR_FLAGS |= USART1.RXDATAH;
	#endif
		
	// Put data into receive buffer
	#if AVR_CLASS == 0
		#if UART_USE_9BITS == 1
			// Get status and 9th bit, then data from buffer
			// Must be read before reading the low bits from UDRn
			if(UCSR1B & (1 << RXB81)){
				uart1.bitnineRX |= ((uint32_t)1 << uart1.uartRXWriteIdx);
			}else{
				uart1.bitnineRX &= ~((uint32_t)1 << uart1.uartRXWriteIdx);
			}
		#endif
	
		uart1.UART_RX_BUFFER[uart1.uartRXWriteIdx] = UDR1;
	#else
	
		#if UART_USE_9BITS == 1
			if(USART1.RXDATAH & 0x01){ // Read high byte first
				uart1.bitnineRX |= ((uint32_t)1 << uart1.uartRXWriteIdx);
			}else{
				uart1.bitnineRX &= ~((uint32_t)1 << uart1.uartRXWriteIdx);
			}
		#endif
	
		uart1.UART_RX_BUFFER[uart1.uartRXWriteIdx] = USART1.RXDATAL;
	#endif
		
	// If there's a user function to handle this receive event
	if(uart1.uartRXfunc){
		// Call it and pass the received data
		uart1.uartRXfunc(uart1.UART_RX_BUFFER[uart1.uartRXWriteIdx]);
	}
		
	uart1.uartRXWriteIdx++;
	uart1.uartBytesToRead++;
}
#endif
#endif