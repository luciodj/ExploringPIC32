/*
**  The UART Matrix
**
** v2.0 LDJ 11/10/07
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <stdlib.h>

#include "CONU2.h"

#define COL         40
#define ROW         23
#define DELAY     3000

main()
{
    int v[40];      // length of each column 
    int i, j, k;
    
    // 1. initializations
    T1CON = 0x8030; // TMR1 on, prescale 256, int clock (Tpb)
    initU2();       // initialize UART (115200, 8N1, CTS/RTS)
    clrscr();       // clear the terminal (VT100 emulation)
    
    // 2. randomize the sequence
    getU2();        // wait for a character input
    srand( TMR1);   // use the current timer value as seed
    
    // 3. init each column lenght
    for( j =0; j<COL; j++)
            v[j] = rand()%ROW;
    
    // 4. main loop
    while( 1)
    {
        home();
        
        // 4.1 refresh the entire screen, one row at a time
        for( i=0; i<ROW; i++)
        {
            // 4.1.1 delay to slow down the screen update
            TMR1 =0;   
            while( TMR1<DELAY);
            
            // 4.1.2 refresh one column at a time
            for( j=0; j<COL; j++)
            {
                // update each column 
                if ( i < v[j])
                    putU2( 33 + (rand()%94));
                else 
                    putU2( ' ');

                // additional column spacing
                putU2( ' ');
            } // for j
            
            // 4.1.3 empty string, advance to next line
            puts("");
        } // for i

        // 4.2 randomly increase or reduce each column lenght
        for( j=0; j<COL; j++)
        {
            switch ( rand()%3)
            {
                case 0: // increase length
                        v[j]++;
                        if (v[j]>ROW)
                            v[j]=ROW;
                        break;
                        
                case 1: // decrease length
                        v[j]--;
                        if (v[j]<1)
                            v[j]=1;
                        break;
                        
                default:// unchanged 
                        break;
             } // switch
        } // for j
    } // main loop
} // main
