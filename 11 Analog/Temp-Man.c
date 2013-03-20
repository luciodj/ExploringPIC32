/*
** Temp-Man.c
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
    initADC( AINPUTS);  // initialize the ADC 
    initLCD();
    
    // 2. use the first reading to randomize 
    srand( readADC( TSENS)); 
    // generate the first random position
    r = rand() % 16; 
    p = '<';
    
    // 3. compute the average value for the initial reference 
    a = 0;
    for ( j=0; j<10; j++)
    {
         a += readADC( TSENS);  // read the temperature
         Delayms( 100);
    }
    i = a / 10;                 // average 

    // main loop
    while( 1)
    {   
        // 4. take the average value over 1 second
        a = 0;
        for ( j=0; j<10; j++)
        {
             a += readADC( TSENS);  // read the temperature
             Delayms( 100);         
        }
        a /= 10;                    // average result
        
        // 5. compare initial reading, move the Pac
        a = 7 + (a - i);
        
        // 6. keep the result in the value range 0..15
        if ( a > 15) 
            a = 15;
        if ( a < 0)
            a = 0;

        // 7. turn the Pac in the direction of movement
        if ( a < n)             // moving to the left
            p = '>';
        if ( a > n)             // moving to the right
            p = '<'; 
                
        // 8. as soon as the Pac eats the food, generate new 
        while (a == r )
            r = rand() % 16;

        // 9. update display
        clrLCD();
        setLCDC( r); putLCD( '*');
        setLCDC( a); putLCD( p);
        
        // 10. remember previous postion
        n = a;

    } // main loop
} // main
