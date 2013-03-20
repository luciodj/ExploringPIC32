/*
** LineTest1.c
**
** testing the basic line drawing function
**
** LDJ rev 2.0 - 6/3/07
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>
#include <graphic.h>

main()
{    
    int x;
    float x0 = 10.0,    y0 = 20.0;
    float x1 = 200.0,   y1 = 150.0;
    float x2 = 20.0,    y2 = 150.0;

    // initializations
    initEX16();     // init and enable vectored interrupts
    clearScreen();  // clear the image buffer
    initVideo();    // start the video state machine
    singleV();      // paint and display same image buffer

    // draw an oblique line (x0,y0) – (x1,y1)
    for( x=x0; x<x1; x++)
        plot( x, y0+(y1-y0)/(x1-x0)* (x-x0));

    // draw a second (steeper) line (x0,y0) – ( x2,y2)
    for( x=x0; x<x2; x++)
        plot( x, y0+(y2-y0)/(x2-x0)* (x-x0));

    // main loop    
	while( 1)
	{   
   
	} // main loop

} // main
