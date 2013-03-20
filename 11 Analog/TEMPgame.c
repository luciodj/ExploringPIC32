/*
** Tempgame.c
** Converting the analog signal from a TC1047 Temp Sensor
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <explore.h>
#include <LCD.h>
#include <ADC.h>


main ()
{
    int a, i, j, n, r, p;
    
    // 1. initializations
    initEX16();
    initADC( AINPUTS);  // initialize the ADC 
    initLCD();
    
    // 2. use the first reading to randomize 
    srand( readADC( TSENS)); 
    // generate the first random position
    r = rand() % 16; 
    p = '<';
    
    // 3. compute the average value for the initial reference 
    a = 0;
    for ( j= 16; j >0; j--)
         a += readADC( TSENS);  // read the temperature
    i = a >> 4;
    
    // 4. main loop
    while( 1)
    {   
        // 4.1 take the average value over 1 second
        Delayms( 1000);         // 1 sec
        a = readADC( TSENS);    // read the temperature
        
        // 4.2 compare initial reading, move the bar 1pos/degC
        a = 7 + (a - i);
        
        // keep the result in the value range 0..15
        if ( a > 15) 
            a = 15;
        if ( a < 0)
            a = 0;

        // turn PIC in direction of movement
        if ( a < n)             // moving to the left
            p = '>';
        if ( a > n)             // moving to the right
            p = '<'; 
                
        // 4.3 as soon as the PIC eats the food, generate new 
        while (a == r )
            r = rand() % 16;

        // update display
        clrLCD();
        setLCDC( r); putLCD( '*');
        setLCDC( a); putLCD( p);
        
        // remember previous postion
        n = a;

    } // main loop
} // main
