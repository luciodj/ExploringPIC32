/*
**  PS2IC.c
**
**  6/11/06  v1.1 LDJ PIC24
**  11/16/07 v1.2 LDJ porting to PIC32
*/

#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>
#include "ps2ic.h"

#define PS2DAT  _RG12       // PS2 Data input pin
#define PS2CLK  _RD8        // PS2 Clock input pin (IC1)

// definition of the keyboard PS/2 state machine 
#define PS2START    0
#define PS2BIT      1
#define PS2PARITY   2
#define PS2STOP     3

#define TPS         (FPB/1000000)   // timer ticks per uS
#define TMAX        500*TPS         // 500uS time out limit

// PS2 KBD state machine and buffer
int PS2State;
unsigned char KBDBuf;
int KCount, KParity;

// mailbox
volatile int KBDReady;
volatile unsigned char KBDCode;


void initKBD( void)
{
    // init I/Os
    _TRISD8 = 1;    // make RD8, IC1 an input pin, PS2 clock
    _TRISG12 = 1;   // make RG12 an input pin, PS2 data

    // clear the kbd flag
    KBDReady = 0;

    // init input capture 
    IC1CON = 0x8082;        // TMR2, int every cap, fall'n edge
    mIC1ClearIntFlag();     // clear the interrupt flag
    mIC1SetIntPriority( 1);
    mIC1IntEnable( 1);      // enable the IC1 interrupt

    // init Timer2
    mT2ClearIntFlag();      // clear the timer interrupt flag
    mT2SetIntPriority( 1);
    mT2IntEnable( 1);       // enable (TMR2 is not active yet)
} // init KBD


void __ISR( _INPUT_CAPTURE_1_VECTOR, ipl1) IC1Interrupt( void)
{ // input capture interrupt service routine
    int d; 
    
    // 1. reset timer on every edge
    TMR2 = 0;

    switch( PS2State){
    default:
    case PS2START:   
        if ( ! PS2DAT)          // verify start bit
        { 
            KCount = 8;         // init bit counter
            KParity = 0;        // init parity check
            PR2 = TMAX;         // init timer period
            T2CON = 0x8000;     // enable TMR2, 1:1 
            PS2State = PS2BIT;
        }
        break;

    case PS2BIT:
        KBDBuf >>=1;            // shift in data bit
        if ( PS2DAT)
            KBDBuf += 0x80;
        KParity ^= KBDBuf;      // update parity
        if ( --KCount == 0)     // if all bit read, move on
            PS2State = PS2PARITY;
        break;

    case PS2PARITY:
        if ( PS2DAT)            // verify parity bit
            KParity ^= 0x80;
        if ( KParity & 0x80)    // if parity odd, continue
            PS2State = PS2STOP;
        else
            PS2State = PS2START;
        break;

    case PS2STOP:    
        if ( PS2DAT)            // verify stop bit
        {
            KBDCode = KBDBuf;   // save code in mail box
            KBDReady = 1;       // set flag, code available
            T2CON = 0;          // stop the timer
        }    
        PS2State = PS2START;
        break;

    } // switch state machine

    // clear interrupt flag
    d = IC1BUF;                // discard capture
    mIC1ClearIntFlag();

} // IC1 Interrupt


void __ISR( _TIMER_2_VECTOR, ipl1) T2Interrupt( void)
{ // timeout
    // reset the state machine
    PS2State = PS2START;

    // stop the timer
    T2CON = 0;
    
    // clear flag and exit
    mT2ClearIntFlag(); 

} // T2 Interrupt
