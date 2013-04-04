/*
**  FFT.h
**	
**  power of two optimized algorithm
*/

#include <xc.h>
#include <math.h>

#define N_FFT   256              // samples must be power of 2
#define PI2N    (2.0 * 3.1415926 / N_FFT)

extern volatile int inCount;

// preparation of the rotation vectors
void initFFT( void);

// input window
void windowFFT( int *source);

// fast fourier transform 
void FFT( void);

// compute power and scale output
void powerScale( int *dest);
