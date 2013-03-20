/*
** NUMB3RS
**
** Example4
*/

#include <p32xxxx.h>

main ()
{
    char        c1, c2, c3;
    short       s1, s2, s3;
    int         i1, i2, i3;
    long long   ll1, ll2, ll3;
    float       f1,f2, f3;
    double      d1, d2, d3;
    
    c1 = 12;        // testing char integers (8-bit)
    c2 = 34;
    c3 = c1 * c2;
    
    s1 = 1234;      // testing short integers (16-bit)
    s2 = 5678;	
    s3= s1 * s2;	

    i1 = 1234567;     // testing (long) integers (32-bit)
    i2 = 3456789;	
    i3= i1 * i2;
    
    ll1 = 1234L;   // testing long long integers (64-bit)
    ll2 = 5678L;	
    ll3= ll1 * ll2;	

    f1 = 12.34;	    // testing single precision floating point
    f2 = 56.78;
    f3= f1 * f2;

    d1 = 12.34L;    // testing double precision floating point
    d2 = 56.78L;	
    d3= d1 * d2;	
} // main
