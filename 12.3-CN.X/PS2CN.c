/*
**  PS2CN.c 
**  
**  06/09/06 v1.0 LDJ PIC24
**  11/16/07 v1.1 LDJ porting to PIC32
*/
#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>
#include "PS2CN.h"

#define PS2DAT  _RG12       // PS2 Data input pin
#define PS2CLK  _RG9        // PS2 Clock input pin (CN11)

// definition of the keyboard PS/2 state machine 
#define PS2START    0
#define PS2BIT      1
#define PS2PARITY   2
#define PS2STOP     3

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
    _TRISG9 = 1;            // make RG9 an input pin 
    _TRISG12 = 1;           // make RG12 an input pin

    // clear the flag
    KBDReady = 0;

    // configure Change Notification system 
    CNENbits.CNEN11 = 1;    // enable PS2CLK (CN11)
    CNCONbits.ON = 1;       // turn on Change Notification
    mCNSetIntPriority( 1);  // set interrupt priority >0
    mCNClearIntFlag();      // clear the interrupt flag
    mCNIntEnable( 1);       // enable interrupt

} // init KBD



void __ISR( _CHANGE_NOTICE_VECTOR, ipl1) CNInterrupt( void)
{ // change notification interrupt service routine

    // 1. make sure it was a falling edge
    if ( PS2CLK == 0)
    {
        switch( PS2State){
        default:
        case PS2START:              // verify start bit
            if ( ! PS2DAT)
            {
                KCount = 8;         // init bit counter
                KParity = 0;        // init parity check
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
            if ( PS2DAT)            // verify parity
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
            }
            PS2State = PS2START;
            break;

        } // switch state machine
    } // if falling edge

    // clear interrupt flag
    mCNClearIntFlag(); 

} // CN Interrupt

