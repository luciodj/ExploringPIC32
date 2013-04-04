/*
** SDMMC.h SD card interface 
** 
**
*/
#define FAIL    FALSE

// insert LED definitions in your code if desired
#define READ_LED            _RA2
#define WRITE_LED           _RA2

// Init ERROR code definitions
#define E_COMMAND_ACK       0x80
#define E_INIT_TIMEOUT      0x81


typedef unsigned  LBA;  // logic block address, 32 bit wide

void initSD( void);     // initializes I/O pins and SPI
int initMedia( void);   // initializes the SD/MMC memory device

int getCD();            // chech card presence
int getWP();            // check write protection tab

int readSECTOR ( LBA, char *);  // reads a block of data 
int writeSECTOR( LBA, char *);  // writes a block of data

