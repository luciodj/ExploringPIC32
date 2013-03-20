/*
** TestDA4.c
**   generating a sinusoidal waveform
**
** 07/10/06 v1.0 LDJ
** 11/25/07 v2.0 PIC32 porting 
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>
#include <math.h>

void initDA( int samplerate)
{
    // init OC1 module 
    OpenOC1( OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE,
            0, 0);

    // init Timer2 mode and period (PR2)
    OpenTimer2( T2_ON | T2_PS_1_1 |  T2_SOURCE_INT, 
                FPB/samplerate);         
    mT2SetIntPriority( 4); 
    mT2ClearIntFlag();
    mT2IntEnable( 1);     
} // initDA

int count;

const short Table[ 100]={
// insert comma separated values here…
410,
435,
460,
484,
509,
533,
557,
580,
602,
624,
645,
664,
683,
701,
718,
733,
747,
760,
771,
781,
790,
797,
802,
806,
809,
809,
809,
806,
802,
797,
790,
782,
772,
760,
747,
733,
718,
701,
684,
665,
645,
624,
603,
580,
557,
534,
510,
485,
460,
435,
410,
385,
360,
335,
311,
287,
263,
240,
217,
196,
175,
155,
136,
118,
102,
86,
72,
59,
48,
38,
29,
22,
17,
13,
10,
10,
10,
13,
16,
22,
29,
37,
47,
58,
71,
85,
101,
117,
135,
154,
173,
194,
216,
238,
261,
285,
309,
333,
358,
383
};

void __ISR( _TIMER_2_VECTOR, ipl4) T2Interrupt( void)
{
//    OC1RS = PR2/2 + PR2/2 * sin(count*2*M_PI/100);
    OC1RS = Table[ count++]; 

    if ( count >= 100)
        count = 0;

    // clear interrupt flag and exit
    mT2ClearIntFlag();
} // T2 Interrupt


main( void)
{
    initEX16();         // init and enable vectored interrupts
    initDA( 44100);     // init the PWM for 44.1kHz
    
    // main loop
    while( 1);

}// main

