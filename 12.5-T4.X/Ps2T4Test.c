/* 
** PS2T4 Test
** 
** LDJ 6/9/06  v1.0
** LDJ 9/17/07 v2.0 PIC32 testing
*/
// configuration bit settings, Fcy=80MHz, Fpb=40MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <explore.h>
#include "PS2KBD.h"


main()
{
    int Key;
    initEX16();             // init and configure interrupts
    initKBD();              // initialization routine
    
    while ( 1)
    {
        if ( KBDReady)      // wait for the flag 
        {
            Key = KBDCode;  // fetch the key code 
            KBDReady = 0;   // clear the flag 
        }
    } // main loop
} //main
