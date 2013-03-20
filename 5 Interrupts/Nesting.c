/*
** Single Vector Interrupt Nesting
** example 3
*/
#include <p32xxxx.h>
#include <plib.h>

int count;

void __ISR( 0, ipl1) InterruptHandler( void)
{
   // 1. re-enable interrupts immediately (nesting)
   asm("ei");

   // 2. check and serve the highest priority first 
   if ( mT3GetIntFlag())
    {
        count++;
        // clear the flag and exit
        mT3ClearIntFlag();
    } // _T3
    
    // 3. check and serve the lower priority 
    else if ( mT2GetIntFlag()) 
    {
        // spend a LOT of time here!
        while( 1);
        
        // before clearing the flag and exiting
        mT2ClearIntFlag();
    } // _T2    
} // Interrupt Handler

main()
{
    // 4. init timers
    PR3 = 20;
    PR2 = 15;
    T3CON = 0x8030;
    T2CON = 0x8030;
    
    // 5. init interrupts
    mT2SetIntPriority( 1);
    mT3SetIntPriority( 3);
    INTEnableSystemSingleVectoredInt();         
    mT2IntEnable( 1);
    mT3IntEnable( 1);
    
    // main loop
    while( 1);
} // main
