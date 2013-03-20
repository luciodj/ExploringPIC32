/*
** GraphicTest3.c
**
** A starry night
**
** 07/02/06 v 1.0 LDJ 
** 11/20/07 v 2.0 LDJ porting to PIC32
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
    initEX16();     // init and enable vectored interrupts
    clearScreen();  // init the video map
    initVideo();    // start the video state machine

    srand( 0x2345);
        
    for( i=0; i<1000; i++)
    {
        plot( rand()%HRES, rand()%VRES);  
    }

    // main loop    
	while( 1)
	{   
   
	} // main loop

} // main
