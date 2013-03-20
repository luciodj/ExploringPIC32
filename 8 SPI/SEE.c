/* 
** SEE Access Library
*/

#include <p32xxxx.h>
#include "see.h"

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


void initSEE( void)
{
    // init the SPI2 peripheral 
    CSEE = 1;                   // de-select the Serial EEPROM
    TCSEE = 0;                  // make SSEE pin output
    SPI2CON = SPI_CONF;         // enable the peripheral
    SPI2BRG = SPI_BAUD;         // select clock speed
}// initSEE


int readStatus( void)
{
    // Check the Serial EEPROM status register
    int i;	
    CSEE = 0;                   // select the Serial EEPROM
    writeSPI2( SEE_STAT);       // send a READ STATUS COMMAND
    i = writeSPI2( 0);          // send/receive
    CSEE = 1;                   // deselect terminate command
    return i;
} // readStatus


int readSEE( int address)
{ // read a 32-bit value starting at an even address

    int i;

    // wait until any work in progress is completed
    while ( readStatus() & 0x1);// check WIP

    // perform a 16-bit read sequence (two byte sequential read)
    CSEE = 0;                   // select the Serial EEPROM
    writeSPI2( SEE_READ);       // read command
    writeSPI2( address >>8);    // address MSB first
    writeSPI2( address & 0xfc); // address LSB (word aligned)
    i = writeSPI2( 0);          // send dummy, read msb
    i = (i<<8)+ writeSPI2( 0);  // send dummy, read lsb
    i = (i<<8)+ writeSPI2( 0);  // send dummy, read lsb
    i = (i<<8)+ writeSPI2( 0);  // send dummy, read lsb
    CSEE = 1;
    return ( i);
}// readSEE


void writeEnable( void)
{
    // send a Write Enable command
    CSEE = 0;               // select the Serial EEPROM
    writeSPI2( SEE_WEN);    // write enable command
    CSEE = 1;               // deselect to complete the command
}// writeEnable   


void writeSEE( int address, int data)
{ // write a 32-bit value starting at an even address

    // wait until any work in progress is completed
    while ( readStatus() & 0x1);// check the WIP flag
  
    // Set the Write Enable Latch
    writeEnable();

    // perform a 32-bit write sequence (4 byte page write)
    CSEE = 0;                   // select the Serial EEPROM
    writeSPI2( SEE_WRITE);      // write command
    writeSPI2( address>>8);     // address MSB first
    writeSPI2( address & 0xfc); // address LSB (word aligned)
    writeSPI2( data >>24);      // send msb
    writeSPI2( data >>16);      // send msb
    writeSPI2( data >>8);       // send msb
    writeSPI2( data);           // send lsb
    CSEE = 1;
}// writeSEE
