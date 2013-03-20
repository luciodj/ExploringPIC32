/*
** Hello Embedded World
** Hello3.c
*/

#include <p32xxxx.h>

main()
{
    // configure all PORTB pins as output
    TRISB =   0;        // all PORTB as output
    PORTB =   0xff;
}
