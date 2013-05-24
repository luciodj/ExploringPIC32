/*
** TextTest.c
**
** 07/03/06 v2.0 LDJ 
** 11/21/07 v3.0 LDJ PIC32 porting
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <xc.h>
#include <explore.h>
#include <graphic.h>



main( void)
{
    int i;
    
    // initializations
    initEX16();     // init and enable vectored interrupts
    initVideo();    // start the state machines
    
    Clrscr();
    
    AT( 5, 2);
    putsV( "Exploring the PIC32!");
        
    AT( 0, 4);
    for( i=0; i<128; i++)
	    putcV( i);

    while (1);

} // main
