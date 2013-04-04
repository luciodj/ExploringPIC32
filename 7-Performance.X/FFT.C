/*
** FFT transform 
**
**
*/

#include "fft.h"


// input complex vector
float   xr[N_FFT];
float   xi[N_FFT];

// rotation vectors
float   wr[N_FFT/2];
float   wi[N_FFT/2];

// bit reversal vector
short   rev[N_FFT/2];

// window 
float   ww[N_FFT];

// apply Hann window to input vector
void windowFFT( int *s)
{
    int i;
    float *xrp, *xip, *wwp;
    
    // apply window to input signal 
    xrp=xr; xip=xi; wwp=ww;
    for( i=0; i<N_FFT; i++)
    { 
        *xrp++ = *s++ * *wwp++;            
        *xip++ = 0;                      
    } // for i
} // windowFFT


void initFFT( void)
{
    int i, m, t, k;
    float *wwp;

    for(i=0; i<N_FFT/2; i++)
    {
        // rotations
        wr[i] = cos(PI2N * i);
        wi[i] = sin(PI2N * i);

        // bit reversal 
        t = i;
        m = 0;
        k = N_FFT-1;
        while (k>0)
        {
            m = (m << 1) + (t & 1);
            t = t >> 1;
            k = k >> 1;
        }
        rev[i] = m;
    } // for i

    // initialize Hanning window vector
    for( wwp=ww, i=0; i<N_FFT; i++)
        *wwp++ = 0.5 - 0.5 * cos(PI2N * i);            

} // initFFT


// Fast Fourier Transform 
void FFT( void)
{
  int     m, k, i, j;
  float   a,b,c,d,wwr,wwi,pr,pi;
 
    // FFT loop
    m = N_FFT/2;
    j = 0;
    while( m > 0)
    { /* log(N) cycle */
        k = 0;
        while( k < N_FFT)
        { // batterflies loop
            for( i=0; i<m; i++)
            { // batterfly
                a = xr[i+k];       b = xi[i+k];
                c = xr[i+k+m];     d = xi[i+k+m];
                wwr = wr[i<<j];    wwi = wi[i<<j];
                pr = a-c;          pi = b-d;

                xr[i+k]   = a + c;
                xi[i+k]   = b + d;
                xr[i+k+m] = pr * wwr - pi * wwi;
                xi[i+k+m] = pr * wwi + pi * wwr;
            } // for i
            k += m<<1 ;
        } // while k
        m >>= 1;
        j++;
    } // while m
} // FFT


void powerScale( int *r)
{
    int i, j;
    float t, max;
    float xrp, xip;

    // compute signal power (in place) and find maximum 
    max = 0;
    for( i=0; i<N_FFT/2; i++)
    {   
        j = rev[ i];
        xrp = xr[ j];
        xip = xi[ j];
        t = xrp*xrp + xip*xip;
        xr[ j]=t;
        if ( t > max)
            max = t;
    }

    // bit reversal, scaling of output vector as unsigned char
    max = 255.0/max;
    for( i=0; i<N_FFT/2; i++)
    {
       t = xr[ rev[i]] * max;
       *r++ = t;
    }

} // powerScale


