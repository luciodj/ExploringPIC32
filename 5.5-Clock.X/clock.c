/*
** A real time clock
**
** example 5
*/

#include <plib.h>

int dSec = 0;
int Sec = 0;
int Min = 0;

// 1. Timer1 interrupt service routine
void __ISR_SINGLE__ T1Interrupt( void)
{
	// 1.1 increment the tens of a second counter
	dSec++;			
	
	if ( dSec > 9) 	// 10 tens in a second
	{ 
        dSec = 0;
        Sec++;		// increment the seconds counter

        if ( Sec > 59)	// 60 seconds make a minute
        {
            Sec = 0;
            Min++;	// increment the minute counter

            if ( Min > 59)// 59 minutes in an hour
                Min = 0;
        } // minutes
    } // seconds

    // 1.2 clear the interrupt flag
    mT1ClearIntFlag();
} //T1Interrupt

main()
{
    // 2.1 init I/Os
    DDPCONbits.JTAGEN = 0;  // disable JTAG port
    TRISA = 0xff00;         // set PORTA LSB as output

    // 2.2 configure Timer1 module
    PR1 = 25000-1;  // set the period register
    T1CON = 0x8030;	// enabled, prescaler 1:256, internal clock 	

    // 2.2 init interrupts, 
    mT1SetIntPriority( 1);
    mT1ClearIntFlag();	    
    INTEnableSystemSingleVectoredInt();         
    mT1IntEnable( 1);

    // 2.3. main loop
    while( 1)
    {
        // your main code here
        PORTA = Sec;     	
    } // main loop
} // main
