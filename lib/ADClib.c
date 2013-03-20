/*
**  ADClib.c
**	
*/
#include <p32xxxx.h>
#include <ADC.h>

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
