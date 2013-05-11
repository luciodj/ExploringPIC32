/*
** Buttons2.c
**
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <explore.h>
#include <LCD.h>

int getK( void)
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
} // getK



main( void) 
{
    char s[16];
    int b;
    
    initLCD();          // init LCD display
    putsLCD( "Press any button\n");

    // main loop
    while( 1)
    {
        b = getK();
        sprintf( s, "Code = %X", b);
        clrLCD();
        putsLCD( s);
        
    } // main loop
    
} // main
