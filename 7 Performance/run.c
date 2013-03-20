/*
** Running.c
**
*/
// configuration bit settings 72MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <p32xxxx.h>
#include <plib.h>
#include "fft.h"


main()
{
    int i, t;
    double f;

    // 1. initializations
        
    //  Sysytem config performance
    SYSTEMConfigPerformance( 72000000L);
   
    // configure PB frequency and the number of wait states
    //SYSTEMConfigWaitStatesAndPB(72000000L);      
        
    // enable the cache for max performance
    //CheKseg0CacheOn();             
    
    // enable instruction prefetch
    //cheConfigure( 0, 0, 3, 3);  
    //mCheConfigure( CHECON | 0x30);
    
    // disable RAM wait states
    //mBMXDisableDRMWaitState();  
        
    // init FFT vectors and constants
    initFFT();

    // test sinusoid
    for (i=0; i<N_FFT; i++)
    {        
        f =  sin(2 * PI2N * i ); 
        inB[ i] = 128 + ( unsigned char) (120.0 * f);
    } // for
    
    // init 32-bit timer4/5
    OpenTimer45( T4_ON | T4_SOURCE_INT, 0);

    WritePeriod45( -1L); //simulator fix

    // clear the 32-bit timer count
    WriteTimer45( 0);

    // perform FFT algorithm 
    windowFFT( inB);
    FFT();
    powerScale( inB);

    // read the timer count 
    t = ReadTimer45();
    f = t/36E6;
    
    
  // 3. infinite loop
    while( f);
    
} // main
