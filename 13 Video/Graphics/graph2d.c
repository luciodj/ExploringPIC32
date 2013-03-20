/*
** graph2d.c
**
** 07/02/06 v1.0 LDJ
** 11/21/07 v2.0 LDJ PIC32 porting
*/
// configuration bit settings
#pragma config POSCMOD=XT, FNOSC=PRIPLL
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <explore.h>
#include <graphic.h>
#include <math.h>

#define X0      10              // graph offset
#define Y0      10
#define NODES   20              // define grid 
#define SIDE    10              
#define STEP    1               // movement increment

typedef struct {
        int x;
        int y;
    } point;

point edge[NODES], prev;

main( void)
{
    int i, j, x, y, z;
    float xf, yf, zf, sf;
    int px, py;
    int xoff, scale;

    // initializations
    initEX16();
    clearScreen();
    initVideo();    

    xoff = 100;

    scale = 75;

    while (1)
    {
        // clear hidden screen 
        clearScreen();

        // draw the x, y and z axes crossing in (X0,Y0)
        line( X0, 10, X0, 10);              // z axis
        line( X0-5, Y0, HRES-10, Y0);       // x axis 
        line( X0-2, Y0-2, X0+120, Y0+120);  // y axis
    
        // init the array of previous egde points
        for( j = 0; j<NODES; j++)
        {
            edge[j].x = X0+ j*SIDE/2;
            edge[j].y = Y0+ j*SIDE/2;
        }
    
        // plot the graph of the function for 
        for( i=0; i<NODES; i++)
        {
            // transform the x range to 0..200 offset 100
            x = i * SIDE;
            xf = (6 * M_PI / 200) * (float)(x-xoff);
            prev.y = Y0;
            prev.x = X0 + x;
    
            for ( j=0; j<NODES; j++)
            { 
                // transform the y range to 0..200 offset 100
                y = j * SIDE;
                yf = (6 * M_PI / 200) * (float)(y-100);
                
                // compute the function
                sf = sqrt( xf * xf + yf * yf);
                zf = 1/(1+ sf) * cos( sf );
                
                // scale the output
                z = zf * scale;
                
                // apply isometric perspective and offset
                px = X0 + x + y/2;
                py = Y0 + z + y/2; 
                
                // plot the point
                plot( px, py);
            
                // draw connecting lines to visualize the grid
                line( px, py, prev.x, prev.y);  
                line( px, py, edge[j].x, edge[j].y);
    
                // update the previous points
                prev.x = px;
                prev.y = py;
                edge[j].x = px;
                edge[j].y = py;
            } // for j
        } // for i
        
        // wait for a button 
        getKEY();
        
    } // main loop
} // main
