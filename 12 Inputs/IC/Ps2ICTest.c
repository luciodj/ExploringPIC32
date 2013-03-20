/* 
** PS2ICTest.c
**
** LDJ 6/9/06  v1.0
** 11/16/07 v2.0 porting to PIC32
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <explore.h>
#include "PS2IC.h"

main()
{
    int Key;
    initEX16();                 // init and enable interrupts
    initKBD();                  // initialization routine

    while ( 1)
    {
        if ( KBDReady)          // wait for the flag 
        {
            Key = KBDCode;      // fetch the key code 
            KBDReady = 0;       // clear the flag 
        }
    } // main loop
} //main
