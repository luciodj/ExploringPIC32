/*
**   WavePlayer.c
**
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF


#include <explore.h>

#include <SDMMC.h>
#include <fileio.h>
#include <LCD.h>
#include "AudioPWM.h"

main( void)
{
    initEX16();
    initLCD();
    putsLCD( "Insert card...\n");
    while ( !getCD());
    Delayms( 100);
    
    if ( !mount())
        putsLCD("Mount Failed");
    else
    {  
        clrLCD();
        putsLCD("Playing...");
        if ( !playWAV( "NELLY.WAV"))
        {
            clrLCD();
            putsLCD("File not found");
        }
    } 

    while( 1)
    {
    } // main loop

} //main 
