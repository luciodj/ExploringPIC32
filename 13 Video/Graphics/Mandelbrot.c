/*
** Mandelbrot.c
**
** Mandelbrot Set graphic demo
**
** 06/10/06 v2.0 LDJ 
** 11/10/07 v3.0 LDJ PIC32 porting
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>
#include <graphic.h>

#define SIZE  VRES
#define MAXIT 64

void mandelbrot( float xx0, float yy0, float w)
{
    float x, y, d, x0, y0, x2, y2;
    int i, j, k;

    // calculate increments
    d = w/SIZE;
    
    // repeat on each screen pixel
    y0 = yy0;
    for (i=0; i<SIZE; i++)
    {
        x0 = xx0;
        for (j=0; j<SIZE; j++)
        {        
            // initialization
              x = x0;
              y = y0;         
              k = 0;
             
             // core iteration
              do {
                x2 = x*x;
                y2 = y*y;
                y = 2*x*y + y0;
                x = x2 - y2 + x0;                        
                k++;
              } while ( (x2 + y2 < 4)  && ( k < MAXIT)); 
            
            // check if the point belongs to the Mandelbrot set
            if ( k & 2) plot( j, i);
            //if ( k == MAXIT) plot( j, i);

            // compute next point x0
            x0 += d;             
         } // for j
         PORTA = i;
         // compute next y0
         y0 += d; 
     } // for i
 } // mandelbrot
 
int main( void)
{
    float x, y, w;
    int c;
    
    // initializations
    initEX16();
    initVideo();    // start the state machines

    // intial coordinates lower left corner of the grid 
    x = -2.0;
    y = -2.0;
    // initial grid size
    w = 4.0;
    
    while( 1)
    {    
        clearScreen();          // clear the screen
        mandelbrot( x, y, w);   // draw new image
       
        // wait for a button to be pressed
        c = getKEY();
        switch ( c){
        case 8:         // first quadrant
            w/= 2;
            y += w;
            break;
            
        case 4:         // second quadrant
            w/= 2;
            y += w;
            x += w;
            break;

        case 2:         // third quadrant
            w/= 2;
            x += w;
            break;

        default:
        case 1:         // fourth quadrant
            w/= 2;
            break;
        } // switch 
        
	} // main loop

} // main
