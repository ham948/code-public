
// See https://www.programming-electronics-diy.xyz/2024/01/defining-fcpu.html
#ifndef F_CPU
	#warning	"F_CPU not defined. Define it in project properties."
#elif F_CPU != 16000000
	#warning	"Wrong F_CPU frequency!"
#endif

#include "uart.h"
#include "fat.h"


int main(void){
	DIR dir;	// directory object
	FILE file;	// file object
	uint8_t return_code = 0;
	uint16_t dirItems = 0;
	
	_delay_ms(4000); // time to start the serial terminal
	
	// Print information on a serial terminal (optional)
	UART_begin(&uart0, 9600, UART_ASYNC, UART_NO_PARITY, UART_8_BIT);
	
	// Mount the memory card
	return_code = FAT_mountVolume();

	// If no error
	if(return_code == MR_OK){
		UART_sendString(&uart0, "Card mounted.\n\r");
		
		// Read label and serial number
		char label[12];
		uint32_t vol_sn = 0;
		FAT_getLabel(label, &vol_sn);
		
		
		// Print label
		UART_sendString(&uart0, "Volume label is: ");
		UART_sendString(&uart0, label);
		UART_sendString(&uart0, "\n\n\r"); // 2x new line character
		
		
		return_code = FAT_openDir(&dir, "Logs");

		if(return_code == FR_OK){
			dirItems = FAT_dirCountItems(&dir);

			for(int i = 0 ; i<dirItems; i++){
				return_code = FAT_findNext(&dir,&file);
				
				if(FAT_attrIsFolder(&file)){
				UART_sendString(&uart0,"folder:");
				UART_sendString(&uart0,FAT_getFilename());
				}else{
				UART_sendString(&uart0,"file:");
				UART_sendString(&uart0,FAT_getFilename());
				}
			UART_sendString(&uart0,"\n\r");
			}
		}
	}

    while (1){
		_delay_ms(1000);
    }
}