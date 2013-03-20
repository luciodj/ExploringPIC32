/*
** Message in a bottle
**
** Explorer16 version (long delays)
*/ 

#include <p32xxxx.h>

// 1. define timing constant
#define SHORT_DELAY 200
#define LONG_DELAY	1600

// 2. declare and initialize an array with the message bitmap 
char bitmap[30] = { 
    0xff,	// H
    0x08,
    0x08,
    0xff,
    0,
    0,
    0xff,	// E
    0x89,
    0x89,
    0x81,
    0,
    0,
    0xff,	// L
    0x80,
    0x80,
    0x80,
    0,
    0,
    0xff,	// L
    0x80,
    0x80,
    0x80,
    0,
    0,
    0x7e,	// O
    0x81,
    0x81,
    0x7e,
    0,
    0
    };

// 3. the main program
main()
{
    // disable JTAG port 
    DDPCONbits.JTAGEN = 0;

    // 3.1 variable declarations
    int i;           // i will serve as the index 

    // 3.2 initialization
    TRISA = 0;      // all PORTA as output
    T1CON = 0x8030; // TMR1 on, prescale 1:256 PB
    PR1 = 0xFFFF;   // max period (not used)

    // 3.3 the main loop   	
    while( 1)
    {
        // 3.3.1 display loop, hand moving to the right
        for( i=0; i<30; i++)
        {	// 3.3.1.1 update the LEDs
            PORTA = bitmap[i];
	
            // 3.3.1.2 short pause
            TMR1 = 0;
            while ( TMR1 < SHORT_DELAY)
            {
            }
        } // for i
	
        // 3.3.2 long pause, hand moving back to the left
        PORTA = 0;      // turn LEDs off
        TMR1 = 	0;
        while ( TMR1 < LONG_DELAY)
        {
        }
    } // main loop
} // main
