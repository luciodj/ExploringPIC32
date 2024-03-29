/*
** SEE Library test
*/ 

// configuration bit settings, Fcy=80MHz, Fpb=10MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_8, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <xc.h>
#include "see.h"

main()
{
    int data;

    // initialize the SPI2 port and CS to access the 25LC256
    initSEE();

    // main loop
    while ( 1)
    {
        // read current content of memory location
        data = readSEE( 16);

        // increment current value
        data++;         // <-set brkpt here	

        // write back the new value
        writeSEE( 16, data);
        //address++;

    } // main loop
} //main
