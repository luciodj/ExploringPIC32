/*
** Message in a bottle
** message3.c
** MORSE CODE version for the PIC32 Starter Kit
*/

#include <xc.h>

// 1. define timing constant
#define DOT_DELAY 18000


// 2. declare and initialize an array with the message bitmap 
char bitmap[] = { 
    // H ....
    1,0,1,0,1,0,1,0,0,0,
    // E .
    1,0,0,0,
    // L .-..
    1,0,1,1,1,0,1,0,1,0,0,0,
    // L .-..
    1,0,1,1,1,0,1,0,1,0,0,0,    
    // ---
    1,1,1,0,1,1,1,0,1,1,1,
    // end of word
    0,0,0,0,0
	};
	
// 3. the main program
main()
{
	// 3.1 variable declarations
	int i;          // i will serve as the index 

	// 3.2 initialization
	TRISD = 0;      // all PORTD as output
	T1CON = 0x8030; // TMR1 on, prescale 1:256 PB=8MHz
    PR1 = 0xFFFF;   // max period (not used)

	// 3.3 the main loop   	
    while( 1)
    {
        // 3.3.1 display loop, spell a letter at a time
        for( i=0; i<sizeof(bitmap); i++)
        {   
            PORTD = bitmap[i];

            // short pause
            TMR1 = 0;
            while ( TMR1 < DOT_DELAY)
            {
            }
        } // for i

    } // main loop
} // main
