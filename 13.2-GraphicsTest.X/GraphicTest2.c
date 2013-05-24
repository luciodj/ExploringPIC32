/*
** GraphicTest2.c
**
** A test pattern
**
** 07/02/06 v 1.0 LDJ 
** 11/20/07 v 2.0 LDJ porting to PIC32
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <xc.h>
#include <explore.h>
#include <graphic.h>

extern int * VA;

main()
{
    int x, y;

    // initializations
    initEX16();
    clearScreen();  // init the video map
    initVideo();    // start the video state machine

    // fill the video memory map with a pattern
    for( y=0; y<VRES; y++)
        for (x=0; x<HRES/32; x++)
            VA[y*HRES/32 + x]= 1;  
    

    // main loop    
	while( 1)
	{   
   
	} // main loop

} // main
