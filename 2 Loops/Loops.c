/*
** Loops.c
*/

#include <p32xxxx.h>

//#define DELAY   36000        // 256ms delay
#define DELAY   36           // reduced delay for simulation

main()
{  
    // 0. initialization
    DDPCONbits.JTAGEN = 0;  // disable JTAGport, free up PORTA    
    TRISA = 0xff00;         // all PORTA as output
    T1CON = 0x8030;         // TMR1 on, prescale 1:256 PB=36MHz
    PR1 = 0xFFFF;           // set period register to max

    // 1. main loop
    while( 1)
    {
        //1.1 turn all LED ON
        PORTA = 0xff;	
        TMR1 = 0;
        while ( TMR1 < DELAY)
        {
            // just wait here
        }

        // 1.2 turn all LED OFF
        PORTA = 0;
        TMR1 = 0;
        while ( TMR1 < DELAY)
        {
            // just wait here
        }
    } // main loop
} // main
