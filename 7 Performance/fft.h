/*
**  FFT.h
**	
**  power of two optimized algorithm
*/

#include <math.h>

#define N_FFT   256              // samples must be power of 2
#define PI2N    2 * M_PI / N_FFT

extern unsigned char inB[];
extern volatile int inCount;

// preparation of the rotation vectors
void initFFT( void);

// input window
void windowFFT( unsigned char *source);

// fast fourier transform 
void FFT( void);

// compute power and scale output
void powerScale( unsigned char *dest);
