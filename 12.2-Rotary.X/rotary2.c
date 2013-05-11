/*
** Rotary2.c
**
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>
#include <LCD.h>

#define ENCHA   _RA9        // encoder channel A
#define ENCHB   _RA10       // encoder channel B
#define TPMS    (FPB/1000)  // PB clock ticks per ms

// state machine definitions
#define R_IDLE      0       
#define R_DETECT    1

volatile int RCount;
static char RState;

void initR( void)
{
    // init state machine
    RCount = 0;             // init counter 
    RState = 0;             // init state machine
    
    // init Timer2
    T2CON = 0x8020;         // enable Timer2, Fpb/4
    PR2 = 5*TPMS/4;         // 5 ms period
    mT2SetIntPriority( 1);     
    mT2ClearIntFlag();      
    mT2IntEnable( 1);
} // init R


void __ISR( _TIMER_2_VECTOR, ipl1) T2Interrupt( void)
{
    static char d;
    
    switch ( RState) {
        default:
        case R_IDLE:       // waiting for CHA rise
            if ( ! ENCHA) 
            {
                RState  = R_DETECT;
                if ( ! ENCHB)
                    d = -1;
            }
            else
                d = 1;
            break;

        case R_DETECT:      // waitin for CHA fall
            if ( ENCHA)
            {
                RState = R_IDLE;
                RCount += d;
            }
            break;
    } // switch
    
    mT2ClearIntFlag();    
} // T2 Interrupt


main( void) 
{
    int i = 0;
    char s[16];
    
    initEX16();             // init and enable interrupts
    initLCD();              // init LCD module
    initR();                // init Rotary Encoder

    // main loop
    while( 1)
    {
        Delayms( 100);
         
        clrLCD();
        sprintf( s, "RCount = %d", RCount);
        putsLCD( s);
        
    } // main loop
    
} // main
