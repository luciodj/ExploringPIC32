/*
**  Matrix2.c
**
**  6/21/07 v3.0 LDJ PIC32 porting
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF
#include <p32xxxx.h>
#include <graphic.h>

#define COL     HRES/8
#define ROW     VRES/8
    
main()
{
    int v[ COL];  // vector containing lengh of each string
    int i,j,k;
    
    // 1. initializations
    initEX16();
    initVideo();
    Clrscr();       // clear the screen
    
    // 2. init each column lenght
    for( j =0; j<COL; j++)
            v[j] = rand()%ROW;
    
    // 3. main loop
    while( 1)
    {
        // 3.1 refresh the screen with random columns
        for( i=0; i<ROW; i++)
        {
            AT( 0, i);
            // refresh one row at a time
            for( j=0; j<COL; j++)
            {
                // fill random char down to each column lenght
                if ( i < v[j])
                    putcV( '!' + (rand()%15));
                else 
                    putcV(' ');
            } // for j            
        } // for i

        // 3.2 randomly increase or reduce each column lenght
        for( j=0; j<COL; j++)
        {
            switch ( rand()%3){
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

        // 3.3 delay to slow down the screen update
        Delayms( 5);
        
        } // for j
    } // main loop
} // main
