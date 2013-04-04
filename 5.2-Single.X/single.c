/*
** Single Interrupt vector test
** example 2
*/
#include <plib.h>

// define shortcuts
#define _T2IE   IEC0bits.T2IE
#define _T2IF   IFS0bits.T2IF
#define _T2IP   IPC2bits.T2IP

int count;

void __ISR_SINGLE__  InterruptHandler( void)
{
    count++;   
    _T2IF = 0;          // equivalent to mT2ClearIntFlag()
} // interrupt handler


main()
{
    // 1. init timers
    PR2 = 15;
    T2CON = 0x8030;
    
    // 2. init interrupts
    _T2IP = 1;
    INTEnableSystemSingleVectoredInt();         
    _T2IE = 1;
    
    // 3. main loop
    while( 1);
} // main
