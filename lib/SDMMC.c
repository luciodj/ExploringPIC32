/*
** SDMMC.c SD card interface 
**  
*/
#include <p32xxxx.h>
#include <explore.h>
#include <sdmmc.h>

// I/O definitions
#define SDWP    _RG1    // Write Protect input
#define SDCD    _RF1    // Card Detect input
#define SDCS    _RF0    // Card Select output

// SD card commands
#define RESET           0 // a.k.a. GO_IDLE (CMD0)
#define INIT            1 // a.k.a. SEND_OP_COND (CMD1)
#define READ_SINGLE     17
#define WRITE_SINGLE    24

// additional commands (not used)
#define SEND_CSD        9
#define SEND_CID        10
#define SET_BLEN        16
#define APP_CMD         55
#define SEND_APP_OP     41

// SD card responses
#define DATA_START      0xFE
#define DATA_ACCEPT     0x05

// timeouts
#define I_TIMEOUT       10000   
#define R_TIMEOUT       25000
#define W_TIMEOUT       250000


void initSD( void)
{
    SDCS = 1;           // initially keep the SD card disabled
    _TRISF0 = 0;        // make Card select an output pin
    
    // init the spi module for a slow (safe) clock speed first
    SPI2CON = 0x8120;   // ON, CKE=1; CKP=0, sample middle
    SPI2BRG = 71;       // clock = Fpb/144 = 250kHz
    
}   // initSD


// send one byte of data and receive one back at the same time
unsigned char writeSPI( unsigned char b)
{
    SPI2BUF = b;                  // write to buffer for TX
    while( !SPI2STATbits.SPIRBF); // wait transfer complete
    return SPI2BUF;               // read the received value
}// writeSPI

#define readSPI()   writeSPI( 0xFF)
#define clockSPI()  writeSPI( 0xFF)
#define disableSD() SDCS = 1; clockSPI()
#define enableSD()  SDCS = 0


int sendSDCmd( unsigned char c, unsigned a)
// c    command code
// a    byte address of data block
{
    int i, r;

    // enable SD card
    enableSD();

    // send a comand packet (6 bytes)
    writeSPI( c | 0x40);    // send command 
    writeSPI( a>>24);       // msb of the address
    writeSPI( a>>16);       
    writeSPI( a>>8);
    writeSPI( a);           // lsb
    
    writeSPI( 0x95);        // send CMD0 CRC 

    // now wait for a response, allow for up to 8 bytes delay
    for( i=0; i<8; i++) 
    {
        r = readSPI();      
        if ( r != 0xFF) 
            break;
    }
    return ( r);         

/* return response
    FF - timeout 
    00 - command accepted
    01 - command received, card in idle state after RESET

other codes:
    bit 0 = Idle state
    bit 1 = Erase Reset
    bit 2 = Illegal command
    bit 3 = Communication CRC error
    bit 4 = Erase sequence error
    bit 5 = Address error
    bit 6 = Parameter error
    bit 7 = Always 0
*/
    // NOTE CSCD is still low!
} // sendSDCmd


int initMedia( void)
// returns 0 if successful
//          E_COMMAND_ACK   failed to acknowledge reset command
//          E_INIT_TIMEOUT  failed to initialize
{
    int i, r;

    // 1. with the card NOT selected     
    disableSD(); 

    // 2. send 80 clock cycles start up
    for ( i=0; i<10; i++)
        clockSPI();

    // 3. now select the card
    enableSD();

    // 4. send a single RESET command
    r = sendSDCmd( RESET, 0); disableSD();
    if ( r != 1)                // must return Idle
        return E_COMMAND_ACK;   // comand rejected

    // 5. send repeatedly INIT until Idle terminates
    for (i=0; i<I_TIMEOUT; i++) 
    {
        r = sendSDCmd( INIT, 0); disableSD();
        if ( !r) 
            break; 
    } 
    if ( i == I_TIMEOUT)   
        return E_INIT_TIMEOUT;  // init timed out 

    // 6. increase speed 
    SPI2CON = 0;                // disable the SPI2 module
    SPI2BRG = 0;                // Fpb/(2*(0+1))= 36/2 = 18MHz
    SPI2CON = 0x8120;           // re-enable the SPI2 module
    
    return 0;           
} // init media


int readSECTOR( LBA a, char *p)
// a        LBA of sector requested
// p        pointer to sector buffer
// returns  TRUE if successful
{
    int r, i;
    
#ifdef READ_LED    
    READ_LED = 0;
#endif
        
    // 1. send READ command
    r = sendSDCmd( READ_SINGLE, ( a << 9));
    if ( r == 0)    // check if command was accepted
    {  
        // 2. wait for a response
        for( i=0; i<R_TIMEOUT; i++)
        {
            r = readSPI();     
            if ( r == DATA_START) 
                break;
        } 

        // 3. if it did not timeout, read 512 byte of data
        if ( i != R_TIMEOUT)
        {
            i = 512;
            do{ 
                *p++ = readSPI();
            } while (--i>0);

            // 4. ignore CRC
            readSPI();
            readSPI();

        } // data arrived

    } // command accepted

    // 5. remember to disable the card
    disableSD();

#ifdef READLED
    READ_LED = 1;
#endif

    return ( r == DATA_START);    // return TRUE if successful
} // readSECTOR


int writeSECTOR( LBA a, char *p)
// a        LBA of sector requested
// p        pointer to sector buffer
// returns  TRUE if successful
{
    unsigned r, i;

    // 0. check Write Protect
    if ( getWP())
        return FAIL;

    // 1. send WRITE command
    r = sendSDCmd( WRITE_SINGLE, ( a << 9));
    if ( r == 0)    // check if command was accepted
    {  
        // 2. send data
        writeSPI( DATA_START);
        
        // send 512 bytes of data
        for( i=0; i<512; i++)
            writeSPI( *p++);

        // 3. send dummy CRC
        clockSPI();
        clockSPI();
    
        // 4. check if data accepted
        r = readSPI(); 
        if ( (r & 0xf) == DATA_ACCEPT)
        {   
#ifdef WRITE_LED    
                WRITE_LED = 0;
#endif

            // 5. wait for write completion
            for( i=0; i<W_TIMEOUT; i++)
            { 
                r = readSPI();
                if ( r != 0 )
                    break;
            } 
#ifdef WRITE_LED
                WRITE_LED = 1;
#endif

        } // accepted
        else
            r = FAIL;

    } // command accepted

    // 6. disable the card
    disableSD();

    return ( r);      // return TRUE if successful

} // writeSECTOR


// SD card connector presence detection switch
int getCD( void) 
// returns  TRUE card present
//          FALSE card not present
{
    return !SDCD;
}


// card Write Protect tab detection switch
int getWP( void)
// returns  TRUE write protect tab on LOCK
//          FALSE write protection tab OPEN
{
    return SDWP;
}
