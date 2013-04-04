/*
** Hello Embedded World
** Hello4.c
*/

#include <xc.h>

main()
{
    // configure all PORTB pins as output
    TRISB =   0;        // all PORTB as output
    AD1PCFG = 0xffff;   // all PORTB as digital
    PORTB =   0xff;
}
