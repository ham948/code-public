#include <avr/io.h>
#include <util/delay.h>
#include "serial_lib.h"

#define DDR_SPI     DDRB
#define PORT_SPI    PORTB
#define CS          PINB2
#define MOSI        PINB3
#define MISO        PINB4
#define SCK         PINB5

#define PARAM_ERROR(X)      X & 0b01000000
#define ADDR_ERROR(X)       X & 0b00100000
#define ERASE_SEQ_ERROR(X)  X & 0b00010000
#define CRC_ERROR(X)        X & 0b00001000
#define ILLEGAL_CMD(X)      X & 0b00000100
#define ERASE_RESET(X)      X & 0b00000010
#define IN_IDLE(X)          X & 0b00000001

#define CMD_VER(X)          ((X >> 4) & 0xF0)
#define VOL_ACC(X)          (X & 0x1F)

#define VOLTAGE_ACC_27_33   0b00000001
#define VOLTAGE_ACC_LOW     0b00000010
#define VOLTAGE_ACC_RES1    0b00000100
#define VOLTAGE_ACC_RES2    0b00001000

#define POWER_UP_STATUS(X)  X & 0x40
#define CCS_VAL(X)          X & 0x40
#define VDD_2728(X)         X & 0b10000000
#define VDD_2829(X)         X & 0b00000001
#define VDD_2930(X)         X & 0b00000010
#define VDD_3031(X)         X & 0b00000100
#define VDD_3132(X)         X & 0b00001000
#define VDD_3233(X)         X & 0b00010000
#define VDD_3334(X)         X & 0b00100000
#define VDD_3435(X)         X & 0b01000000
#define VDD_3536(X)         X & 0b10000000

#define CMD0        0
#define CMD0_ARG    0x00000000
#define CMD0_CRC    0x94

#define CMD8        8
#define CMD8_ARG    0x0000001AA
#define CMD8_CRC    0x86 //(1000011 << 1)

#define CMD58       58
#define CMD58_ARG   0x00000000
#define CMD58_CRC   0x00

#define CMD55       55
#define CMD55_ARG   0x00000000
#define CMD55_CRC   0x00

#define ACMD41      41
#define ACMD41_ARG  0x40000000
#define ACMD41_CRC  0x00

#define SD_SUCCESS  0
#define SD_ERROR    1

#define CMD17                   17
#define CMD17_CRC               0x00
#define SD_MAX_READ_ATTEMPTS    1563

#define CMD24                   24
#define CMD24_ARG               0x00
#define SD_MAX_WRITE_ATTEMPTS   3907
#define CMD24_CRC               0x00

#define SD_R1_NO_ERROR(X)   X < 0x02
#define SD_READY            0x00

#define SD_START_TOKEN          0xFE
#define SD_ERROR_TOKEN          0x00

#define SD_BLOCK_LEN            512

#define CS_ENABLE()     PORT_SPI &= ~(1 << CS)
#define CS_DISABLE()    PORT_SPI |=  (1 << CS)

void SPI_init(){
    DDR_SPI |= (1 << CS) | (1 << MOSI) | (1 << SCK);
    DDR_SPI |= (1 << MISO);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0); 
}

uint8_t SPI_transfer(uint8_t data){
    SPDR = data;

    while(!(SPSR & (1<<SPIF)));

    return SPDR;
}


void SD_powerUpSeq()
{
    // make sure card is deselected
    CS_DISABLE();

    // give SD card time to power up
    _delay_ms(1);

    // send 80 clock cycles to synchronize
    for(uint8_t i = 0; i < 10; i++)
        SPI_transfer(0xFF);

    // deselect SD card
    CS_DISABLE();
    SPI_transfer(0xFF);
}

void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    // transmit command to sd card
    SPI_transfer(cmd|0x40);

    // transmit argument
    SPI_transfer((uint8_t)(arg >> 24));
    SPI_transfer((uint8_t)(arg >> 16));
    SPI_transfer((uint8_t)(arg >> 8));
    SPI_transfer((uint8_t)(arg));

    // transmit crc
    SPI_transfer(crc|0x01);
}

uint8_t SD_readRes1()
{
    uint8_t i = 0, res1;

    // keep polling until actual data received
    while((res1 = SPI_transfer(0xFF)) == 0xFF)
    {
        i++;

        // if no data received for 8 bytes, break
        if(i > 8) break;
    }

    return res1;
}

void SD_readRes7(uint8_t *res)
{
    // read response 1 in R7
    res[0] = SD_readRes1();

    // if error reading R1, return
    if(res[0] > 1) return;

    // read remaining bytes
    res[1] = SPI_transfer(0xFF);
    res[2] = SPI_transfer(0xFF);
    res[3] = SPI_transfer(0xFF);
    res[4] = SPI_transfer(0xFF);
}

void SD_readRes3_7(uint8_t *res)  //read R3 repsonse format but it is just like R7
{
    // read R1
    res[0] = SD_readRes1();

    // if error reading R1, return
    if(res[0] > 1) return;

    // read remaining bytes
    res[1] = SPI_transfer(0XFF);
    res[2] = SPI_transfer(0XFF);
    res[3] = SPI_transfer(0XFF);
    res[4] = SPI_transfer(0XFF);
}


void SD_printR1(uint8_t res)
{
    if(res & 0b10000000)
        { UART_puts("\tError: MSB = 1\r\n"); return; }
    if(res == 0)
        { UART_puts("\tCard Ready\r\n"); return; }
    if(PARAM_ERROR(res))
        UART_puts("\tParameter Error\r\n");
    if(ADDR_ERROR(res))
        UART_puts("\tAddress Error\r\n");
    if(ERASE_SEQ_ERROR(res))
        UART_puts("\tErase Sequence Error\r\n");
    if(CRC_ERROR(res))
        UART_puts("\tCRC Error\r\n");
    if(ILLEGAL_CMD(res))
        UART_puts("\tIllegal Command\r\n");
    if(ERASE_RESET(res))
        UART_puts("\tErase Reset Error\r\n");
    if(IN_IDLE(res))
        UART_puts("\tIn Idle State\r\n");
}


void SD_printR3(uint8_t *res)
{
    SD_printR1(res[0]);

    if(res[0] > 1) return;

    UART_puts("\tCard Power Up Status: ");
    if(POWER_UP_STATUS(res[1]))
    {
        UART_puts("READY\r\n");
        UART_puts("\tCCS Status: ");
        if(CCS_VAL(res[1])){ UART_puts("1\r\n"); }
        else UART_puts("0\r\n");
    }
    else
    {
        UART_puts("BUSY\r\n");
    }

    UART_puts("\tVDD Window: ");
    if(VDD_2728(res[3])) UART_puts("2.7-2.8, ");
    if(VDD_2829(res[2])) UART_puts("2.8-2.9, ");
    if(VDD_2930(res[2])) UART_puts("2.9-3.0, ");
    if(VDD_3031(res[2])) UART_puts("3.0-3.1, ");
    if(VDD_3132(res[2])) UART_puts("3.1-3.2, ");
    if(VDD_3233(res[2])) UART_puts("3.2-3.3, ");
    if(VDD_3334(res[2])) UART_puts("3.3-3.4, ");
    if(VDD_3435(res[2])) UART_puts("3.4-3.5, ");
    if(VDD_3536(res[2])) UART_puts("3.5-3.6");
    UART_puts("\r\n");
}

void SD_printR7(uint8_t *res)
{
    SD_printR1(res[0]);

    if(res[0] > 1) return;

    UART_puts("\tCommand Version: ");
    UART_puthex8(CMD_VER(res[1]));
    UART_puts("\r\n");

    UART_puts("\tVoltage Accepted: ");
    if(VOL_ACC(res[3]) == VOLTAGE_ACC_27_33)
        UART_puts("2.7-3.6V\r\n");
    else if(VOL_ACC(res[3]) == VOLTAGE_ACC_LOW)
        UART_puts("LOW VOLTAGE\r\n");
    else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES1)
        UART_puts("RESERVED\r\n");
    else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES2)
        UART_puts("RESERVED\r\n");
    else
        UART_puts("NOT DEFINED\r\n");

    UART_puts("\tEcho: ");
    UART_puthex8(res[4]);
    UART_puts("\r\n");
}


uint8_t SD_goIdleState()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(CMD0, CMD0_ARG, CMD0_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

void SD_sendIfCond(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD8
    SD_command(CMD8, CMD8_ARG, CMD8_CRC);

    // read response
    SD_readRes7(res);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

void SD_readOCR(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD58
    SD_command(CMD58, CMD58_ARG, CMD58_CRC);

    // read response
    SD_readRes3_7(res);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

uint8_t SD_sendApp()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(CMD55, CMD55_ARG, CMD55_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

uint8_t SD_sendOpCond()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(ACMD41, ACMD41_ARG, ACMD41_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}


uint8_t SD_init()
{
    uint8_t res[5], cmdAttempts = 0;

    SD_powerUpSeq();

    // command card to idle
    while((res[0] = SD_goIdleState()) != 0x01)
    {
        cmdAttempts++;
        if(cmdAttempts > 10) return SD_ERROR;
    }

    // send interface conditions
    SD_sendIfCond(res);
    if(res[0] != 0x01)
    {
        return SD_ERROR;
    }

    // check echo pattern
    if(res[4] != 0xAA)
    {
        return SD_ERROR;
    }

    // attempt to initialize card
    cmdAttempts = 0;
    do
    {
        if(cmdAttempts > 100) return SD_ERROR;

        // send app cmd
        res[0] = SD_sendApp();

        // if no error in response
        if(res[0] < 2)
        {
            res[0] = SD_sendOpCond();
        }

        // wait
        _delay_ms(10);

        cmdAttempts++;
    }
    while(res[0] != SD_READY);

    // read OCR
    SD_readOCR(res);

    // check card is ready
    if(!(res[1] & 0x80)) return SD_ERROR;

    return SD_SUCCESS;
}

uint8_t SD_readSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
    uint8_t res1, read;
    uint16_t readAttempts;

    // set token to none
    *token = 0xFF;

    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD17
    SD_command(CMD17, addr, CMD17_CRC);

    // read R1
    res1 = SD_readRes1();

    // if response received from card
    if(res1 != 0xFF)
    {
        // wait for a response token (timeout = 100ms)
        readAttempts = 0;
        while(++readAttempts != SD_MAX_READ_ATTEMPTS)
            if((read = SPI_transfer(0xFF)) != 0xFF) break;

        // if response token is 0xFE
        if(read == 0xFE)
        {
            // read 512 byte block
            for(uint16_t i = 0; i < 512; i++) *buf++ = SPI_transfer(0xFF);

            // read 16-bit CRC
            SPI_transfer(0xFF);
            SPI_transfer(0xFF);
        }

        // set token to card response
        *token = read;
    }

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

uint8_t SD_writeSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
    uint8_t readAttempts, read, res;

    // set token to none
    *token = 0xFF;

    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD24
    SD_command(CMD24, addr, CMD24_CRC);

    // read response
    res = SD_readRes1();

    // if no error
    if(res == SD_READY)
    {
        // send start token
        SPI_transfer(SD_START_TOKEN);

        // write buffer to card
        for(uint16_t i = 0; i < SD_BLOCK_LEN; i++) SPI_transfer(buf[i]);

        // wait for a response (timeout = 250ms)
        readAttempts = 0;
        while(++readAttempts != SD_MAX_WRITE_ATTEMPTS)
            if((read = SPI_transfer(0xFF)) != 0xFF) { *token = 0xFF; break; }

        // if data accepted
        if((read & 0x1F) == 0x05)
        {
            // set token to data accepted
            *token = 0x05;

            // wait for write to finish (timeout = 250ms)
            readAttempts = 0;
            while(SPI_transfer(0xFF) == 0x00)
                if(++readAttempts == SD_MAX_WRITE_ATTEMPTS) { *token = 0x00; break; }
        }
    }

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res;
}

int main(void)
{
    // array to hold responses
    uint8_t res[5], sdBuf[512], token;
    uint8_t sdBufin[] = "this is a test\nsecond line.";
    

    // initialize UART
    UART_init(103);

    // initialize SPI
    SPI_init();

    // initialize sd card
    if(SD_init() != SD_SUCCESS)
    {
        UART_puts("Error initializaing SD CARD\r\n"); while(1);
    }
    else
    {
        UART_puts("SD Card initialized\r\n");
    }

    SD_writeSingleBlock(0x00000000, sdBufin, &token);

    _delay_ms(1000);


    // read sector 0
    res[0] = SD_readSingleBlock(0x00000000, sdBuf, &token);

    // print response
    if(SD_R1_NO_ERROR(res[0]) && (token == 0xFE))
    {
        for(uint16_t i = 0; sdBufin[i] != '\0'; i++) UART_putc(sdBuf[i]);
        UART_puts("\r\n");
    }
    else
    {
        UART_puts("Error reading sector\r\n");
    }

    while(1) ;
}