/*
** Multiple Vector Interrupt
** example 4
*/
#include <p32xxxx.h>
#include <plib.h>

int count;

void __ISR( _TIMER_3_VECTOR, ipl7) T3InterruptHandler( void)
{
    // 1. T3 handler is responsible for incrementing count
    count++;
    
    // 2. clear the flag and exit
    mT3ClearIntFlag();
} // T3 Interrupt Handler
    
void __ISR( _TIMER_2_VECTOR, ipl1) T2InterruptHandler( void)
{
    // 3. re-enable interrupts immediately (nesting)
    asm("ei");

    // 4. T2 handler code here
    while( 1);
    
    // 5. clear the flag and exit
    mT2ClearIntFlag();
} // T2 Interrupt Handler

main()
{
    // 5. init timers
    PR3 = 20;
    PR2 = 15;
    T3CON = 0x8030;
    T2CON = 0x8030;
    
    // 6. init interrupts
    mT2SetIntPriority( 1);
    mT3SetIntPriority( 7);
    INTEnableSystemMultiVectoredInt();         
    mT2IntEnable( 1);
    mT3IntEnable( 1);
    
    // 7. main loop
    while( 1);
} // main
