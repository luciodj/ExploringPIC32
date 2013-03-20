/*
** Rotary.c
**
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <explore.h>
#include <LCD.h>

#define ENCHA   _RA9        // channel A
#define ENCHB   _RA10       // channel B

main( void) 
{
    int i = 0;
    char s[16];
    
    initLCD();

    // main loop
    while( 1)
    {
        while( ENCHA);     // detect CHA falling edge
        Delayms( 5);       // debounce 
        i += ENCHB ? 1 : -1; 
        while( !ENCHA);    // wait for CHA rising edge
        Delayms( 5);       // debounce
        
        // display relative counter value
        clrLCD();
        sprintf( s, "%d", i);
        putsLCD( s);        
    } // main loop
    
} // main
