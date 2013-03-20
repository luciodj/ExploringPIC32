/*
** graph1d.c
**
** Plotting a 1D function graph
** v1.00 LDJ 7/2/06
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>
#include <graphic.h>
#include <math.h>

#define X0 10
#define Y0 (VRES/2)

main( void)
{
    int x, y;
    float xf, yf;

    // initializations
    initEX16();     // init and enable vectored interrupts
    clearScreen();
    initVideo();    // init video state machine

    // draw the x and y axes crossin in (X0,Y0)
    line( X0, 10, X0, VRES-10);     // y axes
    line( X0-5, Y0, HRES-10, Y0);   // x axes 
    
    // plot the graph of the function for 
    for( x=0; x<200; x++)
    {
        xf = (2 * M_PI / 50) * (float) x;
        yf =  75.0 / ( 8 * M_PI) * xf * sin( xf);
        plot( x+X0, yf+Y0);
    }

    // main loop
    while( 1);

} // main
