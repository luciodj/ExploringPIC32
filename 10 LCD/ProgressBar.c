/*
** Progress Bar
**
** 11/12/07 v1.0 LDJ 
*/
// configuration bit settings, Fcy=72MHz, Fpb=36MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <explore.h>
#include <LCD.h>

#define BRICK   0xff        // last character in LCD font
#define TIP     0x00        // first user defined character

void newBarTip( int i, int width)
{
    char bar; 
    int pos;
    
    // save cursor position
    while( busyLCD());
    pos = addrLCD();

    // generate a new character at position i
    // set the data pointer to the LCD CGRAM buffer
    setLCDG( i*8);
    
    // as a horizontal bar (0-4)x thick moving left to right
    // 7 pixel tall
    if ( width > 4)
        width = 0;
    else 
        width = 4 - width;
        
    for( bar=0xff; width > 0; width--)
        bar<<=1;            // bar >>= 1; if right to left

    // fill each row (8) with the same pattern
    putLCD( bar);
    putLCD( bar);
    putLCD( bar);
    putLCD( bar);
    putLCD( bar);
    putLCD( bar);
    putLCD( bar);
    putLCD( bar);          
    
    // restore cursor position
    setLCDC( pos);
} // newBarTip


void drawProgressBar( int index, int imax, int size)
{   // index is the current progress value
    // imax  is the maximum value 
    // size  is the number of character positions available
    int i;
    
    // scale the input values in the available space
    int width = index * (size*5) / imax;
        
    // generate a character to represent the tip
    newBarTip( TIP, width % 5);   // user defined character 0   

    // draw a bar of solid blocks
    for ( i=width/5; i>0; i--)
        putLCD( BRICK);          // filled block character

    // draw the tip of the bar
    putLCD( TIP);                // use character 0
    
} // drawProgressBar

        
main( void)
{
    int index;
    char s[8];
    
    // LCD initialization
    initLCD();

    index = 0;
    
    // main loop
    while( 1)
    {
        clrLCD();
        
        sprintf( s, "%2d", index);
        putsLCD( s); putLCD( '%');
        
        // draw bar 
        drawProgressBar( index, 100, HLCD-3);
        
        // advance and keep index in boundary
        index++;
        if ( index > 99) 
            index = 0;

        // slow down the action 
        Delayms( 100);
        
    } // main loop
} // main
