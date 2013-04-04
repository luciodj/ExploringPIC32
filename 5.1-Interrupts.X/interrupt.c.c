/*
** Single Interrupt Vector test
** interrupt.c
*/
#include <plib.h>   // plib automatically includes xc.h

int count;

#pragma interrupt InterruptHandler ipl1 vector 0
void  InterruptHandler( void)
{
    count++;
    mT2ClearIntFlag();
} // Interrupt Handler

main()
{
    // 1. init timers
    PR2 = 15;
    T2CON = 0x8030;
    
    // 2. init interrupts
    mT2SetIntPriority( 1);
    INTEnableSystemSingleVectoredInt();         
    mT2IntEnable( 1);
    
    // 3. main loop
    while( 1);
} // main

