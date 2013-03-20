/* 
** Explore.c
**
** v1.0 LDJ 8/2/07
** V2.0 LDJ 10/22/07 rev B libraries
** V2.1 LDJ 10/23/07 removed RAM wait states, enable pre-fetch
*/

#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>

void initEX16( void)
{
    // 1. disable the JTAG port to make the LED bar 
    // available if not using the Starter Kit
#ifndef PIC32_STARTER_KIT
    mJTAGPortEnable( 0);
#endif
    
    // 2. Sysytem config performance
    SYSTEMConfigPerformance( FCY);
   
    // 7. allow vectored interrupts
    INTEnableSystemMultiVectoredInt();   // Interrupt vectoring

    // 8. PORTA output LEDs0..6, make RA7 an input button
    LATA = 0;
    TRISA = 0xFF80;
    
} // initEX16

//
void _general_exception_handler( unsigned c, unsigned s)
{
    while (1);
} // exception handler
//

/*
** Simple Delay functions
**
** uses:    Timer1
** Notes:   Blocking function
*/

void Delayms( unsigned t)
{
    T1CON = 0x8000;     // enable TMR1, Tpb, 1:1
    while (t--)
    {  // t x 1ms loop
        TMR1 = 0;
        while (TMR1 < FPB/1000);
    }
} // Delayms


/*
**
**	Buttons read and debounce
*/

int readKEY( void)
{   // returns 0..F if keys pressed, 0 = none
    int c = 0;

    if ( !_RD6) // leftmost button 
        c |= 8;
    if ( !_RD7)
        c |= 4;
    if ( !_RA7)
        c |= 2;
    if ( !_RD13) // rightmost button
        c |= 1;

    return c;
} // readKEY


int getKEY( void)
{   // wait for a key pressed and debounce
    int i=0, r=0, j=0;
    int c;

    // 1. wait for a key pressed for at least .1sec
    do{
        Delayms( 10);
        if ( (c = readKEY()))
        {
            if ( c>r)       // if more than one button pressed
                r = c;      // take the new code
            i++;    
        }
        else 
            i=0;
    } while ( i<10);

    // 2. wait for key released for at least .1 sec
    i =0;
    do {
        Delayms( 10);   
        if ( (c = readKEY()))
        {
            if (c>r)        // if more then one button pressed 
                r = c;      // take the new code
            i=0;            
            j++;            // keep counting 
        }
        else 
            i++;
    } while ( i<10);        
    
    // 3. check if a button was pushed longer than 500ms
    if ( j>50)              
        r+=0x80;            // add a flag in bit 7 of the code

    // 4. return code
    return r;
} // getKEY



