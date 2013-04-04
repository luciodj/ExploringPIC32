/*
** SPI2
** 
*/
#include <xc.h>

// configuration bit settings, Fcy=80MHz, Fpb=10MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_8, FWDTEN=OFF, CP=OFF, BWP=OFF

// I/O definitions
#define CSEE    _RD12       // select line for Serial EEPROM
#define TCSEE   _TRISD12    // tris control for CSEE pin

// peripheral configurations
#define SPI_CONF    0x8120  // SPI on, 8-bit master,CKE=1,CKP=0
#define SPI_BAUD    15      // clock divider Fpb/(2 * (15+1))

// 25LC256 Serial EEPROM commands
#define SEE_WRSR    1       // write status register	
#define SEE_WRITE   2       // write command
#define SEE_READ    3       // read command
#define SEE_WDI     4       // write disable
#define SEE_STAT    5       // read status register
#define SEE_WEN     6       // write enable

// send one byte of data and receive one back at the same time
int writeSPI2( int i)
{
    SPI2BUF = i;                  // write to buffer for TX
    while( !SPI2STATbits.SPIRBF); // wait for transfer complete
    return SPI2BUF;               // read the received value
}//writeSPI2


main()
{
    int i;

    // 1. init the SPI peripheral 
    TCSEE = 0;                  // make SSEE pin output
    CSEE = 1;                   // de-select the Serial EEPROM
    SPI2CON = SPI_CONF;         // select mode and enable SPI2
    SPI2BRG = SPI_BAUD;         // select clock speed

    // 2. main loop
    while( 1)
    {
    	// 2.1 send a Write Enable command
        CSEE = 0;               // select the Serial EEPROM
        writeSPI2( SEE_WEN);    // write enable command
        CSEE = 1;               // deselect to complete command

        // 2.2 Check the Serial EEPROM status 
        CSEE = 0;               // select the Serial EEPROM
        writeSPI2( SEE_STAT);   // send a READ STATUS COMMAND
        i = writeSPI2( 0);      // send/receive
        CSEE = 1;               // deselect terminate command
    } // main loop 
} // main
