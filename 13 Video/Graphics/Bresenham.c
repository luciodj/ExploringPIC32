/*
** Bresenham.c
**
** Fast line drawing algorithm example
** 
** 11/21/07 v2.0 LDJ PIC32 porting
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
    int i;

    // initializations
    initEX16();     // init and enable vectore interrupts
    initVideo();    // start the state machines
    
    // main loop
	while( 1)
	{   
        clearScreen();
        line( 0, 0, 0, VRES-1);
        line( 0, VRES-1, HRES-1, VRES-1);
        line( HRES-1, VRES-1, HRES-1, 0);
        line( HRES-1, 0, 0, 0);
    
        for( i = 0; i<100; i++) 	
            line( rand()%HRES, rand()%VRES, 
                  rand()%HRES, rand()%VRES);

        // wait for a button to be pressed
        getKEY();
	} // main loop

} // main
