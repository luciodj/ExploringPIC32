/*
** It's an analog world
**
*/
// configuration bit settings, Fcy=80MHz, Fpb=40MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <explore.h>
#include <LCD.h>

#define POT     5       // 10k potentiometer on AN5 input
#define TSENS   4       // TC1047 Temperature sensor on AN4
#define AINPUTS 0xffcf  // Analog inputs for POT and TSENS

// initialize the ADC for single conversion, select input pins
void initADC( int amask) 
{
    AD1PCFG = amask;    // select analog input pins
    AD1CON1 = 0x00E0;   // auto convert after end of sampling 
    AD1CSSL = 0;        // no scanning required 
    AD1CON2 = 0;        // use MUXA, AVss/AVdd used as Vref+/-
    AD1CON3 = 0x1F3F;   // max sample time = 31Tad
    AD1CON1SET = 0x8000;// turn on the ADC
} //initADC


int readADC( int ch)
{
    AD1CHSbits.CH0SA = ch;      // select analog input channel
    AD1CON1bits.SAMP = 1;       // start sampling
    while (!AD1CON1bits.DONE);  // wait to complete conversion
    return ADC1BUF0;            // read the conversion result
} // readADC


main ()
{
    int i, a;
    
    // initializations
    void initEX16( void);   // initialize the Explorer16
    initADC( AINPUTS);      // initialize the ADC
    initLCD();              // initialize the LCD display
        
    // main loop
    while( 1)
    {   
        a = readADC( POT);  // select the POT input and convert 

        // reduce the 10-bit result to a 4 bit value (0..15)
        // (divide by 64 or shift right 6 times
        a >>= 6;
        
        // draw a simple bar on the display
        clrLCD();
        for ( i=0; i<=a; i++) 
            putLCD( 0xFF);
        
        // slow down to avoid flickering
        Delayms( 200);
    } // main loop
} // main
