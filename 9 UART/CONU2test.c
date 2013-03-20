/*
** CONU2 Test
** UART2 RS232 asynchronous communication demonstration code
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <stdlib.h>

#include "conU2.h"


#define BUF_SIZE 128

main()
{
    int i = RAND_MAX;
    char s[BUF_SIZE];
    
	// 1. init the console serial port 
	initU2();
	
    // 2. text prompt 
    clrscr();
    home();
    puts( "Exploring the PIC32!");
    sprintf( s, "Exploring the PIC32! %d", 17);
    puts( s);
    
    // 3. main loop
    while ( 1)
    {
        // 3.1 read a full line of text
        getsn( s, sizeof(s));
        // 3.2 send a string to the serial port
        puts( s);
    } // main loop
}// main
