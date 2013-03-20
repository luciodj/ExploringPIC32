/*
** Pot-Man.c
** 
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
    int a, r, p, n;
    
    // 1. initializations
    initLCD();
    initADC( AINPUTS);  
    
    // 2. use the first reading to randomize 
    srand( readADC( POT)); 
    
    // 3. init the hungry Pac 
    p = '<';
        
    // 4. generate the first random food bit position
    r = rand() % 16; 
    
    // main loop
    while( 1)
    {   
        // 5. select the POT input and convert 
        a = readADC( POT);  

        // 6. reduce the 10-bit result to a 4 bit value (0..15)
        // (divide by 64 or shift right 6 times
        a >>= 6;
        
        // 7. turn the Pac in the direction of movement
        if ( a < n)     // moving to the left
            p = '>';
        if ( a > n)     // moving to the right
            p = '<'; 

        // 8. when the Pac eats the food, generate more food 
        while (a == r )
            r = rand() % 16;
          
        // 9. update display 
        clrLCD();
        setLCDC( a); putLCD( p);
        setLCDC( r); putLCD( '*');

        // 10. provide timing and relative position
        Delayms( 200);      // limit game speed
        n = a;              // memorize previous position
    } // main loop
} // main


