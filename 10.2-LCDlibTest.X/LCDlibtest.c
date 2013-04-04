/*
** LCDlibTest.c
**
*/
// configuration bit settings, Fcy=80MHz, Fpb=40MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF


#include <explore.h>
#include <LCD.h>

main()
{
    initEX16();
    initLCD();
    
    clrLCD();
    putsLCD( "Exploring \nthe \tPIC32");
    
    while( 1);
}
