/*
** Buttons.c
**
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <explore.h>
#include <LCD.h>

/*
**
**	Buttons read and debounce
*/

int readK( void)
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
} // readK



main( void) 
{
    char s[16];
    int b;
    
    initLCD();          // init LCD display

    // main loop
    while( 1)
    {
        clrLCD();
        putsLCD( "Press any button\n");
        b = readK();
        sprintf( s, "Code = %X", b);
        putsLCD( s);
        Delayms( 100);
        
    } // main loop
    
} // main
