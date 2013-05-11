/*
** bounce.c
**
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF
#include <p32xxxx.h>

main( void) 
{
    int count;          // the bounces counter
    
    count = 0;
    
    // main loop
    while( 1)
    {
        // wait for the button to be pressed
        while ( _RD6);      
        
        // count one more button press
        count++;
        
        // wait for the button to be released
        while ( ! _RD6);
                
    } // main loop
    
} // main
