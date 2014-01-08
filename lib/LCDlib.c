/*
**
** LCDlib.c
**
*/

#include <plib.h>
#include <explore.h>

#include <LCD.h>

#define PMDATA  PMDIN


void initLCD( void)
{
    // PMP initialization 
    mPMPOpen( PMP_ON | PMP_READ_WRITE_EN | 3,
              PMP_DATA_BUS_8 | PMP_MODE_MASTER1 | 
              PMP_WAIT_BEG_4 | PMP_WAIT_MID_15 | 
              PMP_WAIT_END_4,
              0x0001,           // only PMA0 enabled 
              PMP_INT_OFF);     // no interrupts used       
        
    // wait for >30ms
    Delayms( 30);
    
    //initiate the HD44780 display 8-bit init sequence
    PMPSetAddress( LCDCMD);     // select command register
    PMPMasterWrite( 0x38);      // 8-bit int, 2 lines, 5x7
    Delayms( 1);                // > 48 us 
    
    PMPMasterWrite( 0x0c);      // ON, no cursor, no blink 
    Delayms( 1);                // > 48 us 
    
    PMPMasterWrite( 0x01);      // clear display
    Delayms( 2);                // > 1.6ms   
    
    PMPMasterWrite( 0x06);      // increment cursor, no shift
    Delayms( 2);                // > 1.6ms   
} // initLCD


char readLCD( int addr)
{
    PMPSetAddress( addr);       // select register
    mPMPMasterReadByte();       // initiate read sequence
    return mPMPMasterReadByte();// read actual data
} // readLCD


void writeLCD( int addr, char c)    
{
    while( busyLCD());
    PMPSetAddress( addr);       // select register
    PMPMasterWrite( c);         // initiate write sequence
} // writeLCD
    

void putsLCD( char *s)
{  
    char c;
    
    while( *s) 
    {
        switch (*s)
        {
        case '\n':          // point to second line
            setLCDC( 0x40);
            break;
        case '\r':          // home, point to first line
            setLCDC( 0);
            break;
        case '\t':          // advance next tab (8) positions
            c = addrLCD();
            while( c & 7)
            {
                putLCD( ' '); 
                c++;
            }
            if (( c > 15)&&( c < 0x40))   // if necessary move to second line
                setLCDC( 0x40);
            break;                
        default:            // print character
            putLCD( *s);
            break;
        } //switch
        s++;
    } //while 
} //putsLCD



