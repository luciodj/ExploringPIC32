/*
** GraphicTest.c
**
** A dark Screen
**
** 07/01/06 v 1.0 LDJ
** 06/03/07 v 2.0 LDJ PIC32 porting
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <xc.h>
#include <explore.h>
#include <graphic.h>

main()
{
    // initializations
    initEX16();     // init and enable vectored interrupts
    clearScreen();  // init the video map
    initVideo();    // start the video state machine

    // main loop    
	while( 1)
	{   
   
	} // main loop

} // main
