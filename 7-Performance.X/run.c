/*
** Run.c
**
*/
// configuration bit settings Fsys = 80MHz, Fpb = 40MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <plib.h>
#include "fft.h"

// input vector
int inB[ N_FFT];

main()
{
    int i, t;
    float f;

    // 1. initializations
        
    //  Sysytem config performance
    SYSTEMConfigPerformance( 80000000L);
   
    // configure PB frequency and the number of wait states
    //SYSTEMConfigWaitStatesAndPB(80000000L);
        
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
        f =  sinf( 15.0 *  PI2N * i );
        inB[ i] = 125.0 * f;
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
    f = t/40E6;
    
    
  // 3. infinite loop
    while( f);
    
} // main
