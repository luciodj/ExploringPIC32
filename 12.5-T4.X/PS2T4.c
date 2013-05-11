/*
**  PS2T4.c
**  
**  06/01/06 v1.0 LDJ PIC24 
**  11/16/07 v1.1 LDJ porting to PIC32
*/
#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>
#include "PS2KBD.h"

#define PS2CLK  _RG13           // PS2 Clock output 
#define PS2DAT  _RG12           // PS2 Data input pin

#define TPS        (FPB/1000000)// ticks per microsecond
#define KMAX        500*TPS     // 500us timout
#define KB_SIZE     16          // kbd buffer size

// definition of the keyboard PS/2 state machine 
#define PS2START    0
#define PS2BIT      1
#define PS2PARITY   2
#define PS2STOP     3

// PS2 KBD state machine and buffer
int PS2State;
unsigned char KBDBuf;
int KState, KCount, KParity, KTimer, KBR, KBW;

// circular buffer
unsigned char KCB[ KB_SIZE];

// mailbox
volatile int KBDReady;
volatile unsigned char KBDCode;


void initKBD( void)
{
    // init I/Os
    ODCGbits.ODCG13 = 1;    // make RG13 open drain (PS2clk)
    _TRISG13 = 1;           // make RG13 an input pin (for now)
    _TRISG12 = 1;           // make RG12 an input pin

    // init the circular buffer pointers
    KBR = 0;
    KBW = 0;

    // clear the kbd flag
    KBDReady = 0;

    // configure Timer4
    PR4 = 25*TPS - 1;       // 25 us
    T4CON = 0x8000;         // T4 on, prescaler 1:1
    mT4SetIntPriority( 1);  // lower priority
    mT4ClearIntFlag();      // clear interrupt flag
    mT4IntEnable( 1);       // enable interrupt 

} // init KBD



void __ISR( _TIMER_4_VECTOR, ipl1) T4Interrupt( void)
{
    int d, k;

//_RA2 =1;

    // 1. check if buffer available 
    if ( !KBDReady && ( KBR!=KBW))
    {
        KBDCode = KCB[ KBR++];
        KBR %= KB_SIZE;
        KBDReady = 1;                   // flag code available
    }
    
    // 2. sample the inputs clock and data at the same time
    d = PS2DAT;
    k = PS2CLK;
    
    // 3. Keyboard state machine
    if ( KState) 
    {   // previous time clock was high KState 1
        if ( !k)                        // PS2CLK == 0 
        {   // falling edge detected, 
            KState = 0;                 // transition to State0
            KTimer = KMAX;              // restart the counter
        
            switch( PS2State){
            default:
            case PS2START:   
                if ( !d)                // PS2DAT == 0
                { 
                    KCount = 8;         // init bit counter
                    KParity = 0;        // init parity check
                    PS2State = PS2BIT;
                }
                break;

            case PS2BIT:      
                KBDBuf >>=1;            // shift in data bit
                if ( d)                 // PS2DAT == 1
                    KBDBuf += 0x80;
                KParity ^= KBDBuf;      // calculate parity
                if ( --KCount == 0)     // all bit read
                    PS2State = PS2PARITY;
                break;

            case PS2PARITY:         
                if ( d)                 // PS2DAT == 1
                    KParity ^= 0x80;
                if ( KParity & 0x80)    // parity odd, continue
                    PS2State = PS2STOP;
                else 
                    PS2State = PS2START;   
                break;

            case PS2STOP:    
                if ( d)                 // PS2DAT == 1
                {
                    KCB[ KBW] = KBDBuf; // write in the buffer
                    // check if buffer full
                    if ( (KBW+1)%KB_SIZE != KBR) 
                        KBW++;          // else increment ptr
                    KBW %= KB_SIZE;     // wrap around
                }    
                PS2State = PS2START;
                break;

            } // switch
        } // falling edge
        else 
        { // clock still high, remain in State1
            KTimer--;
            if ( KTimer ==0)            // timeout
                PS2State = PS2START;    // reset data SM
        } // clock still high
    } // Kstate 1
    else 
    { // Kstate 0
        if ( k)                         // PS2CLK == 1
        { // rising edge, transition to State1
            KState = 1;
        } // rising edge
        else 
        { // clocl still low, remain in State0
            KTimer--;
            if ( KTimer == 0)           // timeout
                PS2State = PS2START;    // reset data SM
        } // clock still low
    } // Kstate 0

    // 4. clear the interrupt flag
    mT4ClearIntFlag();
//_RA2 = 0;
} // T4 Interrupt


// PS2 keyboard codes (standard set #2)
const char keyCodes[128]={    
                0,  F9,   0,  F5,  F3,  F1,  F2, F12,   //00
                0, F10,  F8,  F6,  F4, TAB, '`',   0,   //08
                0,   0,L_SHFT, 0,L_CTRL,'q','1',   0,   //10
                0,   0, 'z', 's', 'a', 'w', '2',   0,   //18
                0, 'c', 'x', 'd', 'e', '4', '3',   0,   //20
                0, ' ', 'v', 'f', 't', 'r', '5',   0,   //28
                0, 'n', 'b', 'h', 'g', 'y', '6',   0,   //30
                0,   0, 'm', 'j', 'u', '7', '8',   0,   //38
                0, ',', 'k', 'i', 'o', '0', '9',   0,   //40
                0, '.', '/', 'l', ';', 'p', '-',   0,   //48
                0,   0,'\'',   0, '[', '=',   0,   0,   //50
          CAPS, R_SHFT,ENTER, ']',  0,0x5c,   0,   0,   //58
                0,   0,   0,   0,   0,   0, BKSP,  0,   //60
                0, '1',   0, '4', '7',   0,   0,   0,   //68
                0, '.', '2', '5', '6', '8', ESC, NUM,   //70
              F11, '+', '3', '-', '*', '9',   0,   0    //78
            };
const char keySCodes[128] = {
                0,  F9,   0,  F5,  F3,  F1,  F2, F12,   //00
                0, F10,  F8,  F6,  F4, TAB, '~',   0,   //08
                0,   0,L_SHFT, 0,L_CTRL,'Q','!',   0,   //10
                0,   0, 'Z', 'S', 'A', 'W', '@',   0,   //18
                0, 'C', 'X', 'D', 'E', '$', '#',   0,   //20
                0, ' ', 'V', 'F', 'T', 'R', '%',   0,   //28
                0, 'N', 'B', 'H', 'G', 'Y', '^',   0,   //30
                0,   0, 'M', 'J', 'U', '&', '*',   0,   //38
                0, '<', 'K', 'I', 'O', ')', '(',   0,   //40
                0, '>', '?', 'L', ':', 'P', '_',   0,   //48
                0,   0,'\"',   0, '{', '+',   0,   0,   //50
          CAPS, R_SHFT,ENTER, '}',  0, '|',   0,   0,   //58
                0,   0,   0,   0,   0,   0, BKSP,  0,   //60
                0, '1',   0, '4', '7',   0,   0,   0,   //68
                0, '.', '2', '5', '6', '8', ESC, NUM,   //70
              F11, '+', '3', '-', '*', '9',   0,   0    //78
            };

/*                  R ALT PRNT R CTRL L GUI R GUI APPS KP / KP EN END
                    L ARROW HOME INSERT DELETE D ARROW R ARROW U ARROW PG DN
                    SCRN PG UP PAUSE 
*/

int CapsFlag=0;

char getcKBD( void)
{
    unsigned char c;
    
    while( 1)
    {
        while( !KBDReady);      // wait for a key to be pressed
        // check if it is a break code
        while (KBDCode == 0xf0)     
        {   // consume the break code
            KBDReady = 0; 
            // wait for a new key code
            while ( !KBDReady);
            // check if the shift button is released
            if ( KBDCode == L_SHFT)
                CapsFlag = 0;
            // and discard it
            KBDReady = 0;       
            // wait for the next key 
            while ( !KBDReady);
        }
        // check for special keys
        if ( KBDCode == L_SHFT)
        {    
            CapsFlag = 1;
            KBDReady = 0;
        }
        else if ( KBDCode == CAPS)
        {
            CapsFlag = !CapsFlag;
            KBDReady = 0;
        }
    
        else // translate into an ASCII code
        {        
            if ( CapsFlag)
                c = keySCodes[KBDCode%128];
            else
                c = keyCodes[KBDCode%128];
            break;
        }
    }
    // consume the current character
    KBDReady = 0;
    
    return ( c);
} // getcKBD
