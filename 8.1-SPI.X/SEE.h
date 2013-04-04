/*
** SEE Access library
** 
** encapsulates 25LC256 Serial EEPROM 
** as a NVM storage device for PIC32 + Explorer16 applications
*/

// initialize access to memory device
void initSEE(void);

// 32-bit integer read and write functions
// NOTE: address must be an even value between 0x0000 and 0x3ffc
// (see page write restrictions on the device datasheet)
int  readSEE ( int address);
void writeSEE( int address, int data);
