/*
** Hello Embedded World
** Hello1.c
*/

#include <p32xxxx.h>

main()
{
    // configure all PORTA pins as output
    TRISA =   0;        // all PORTA as output
    PORTA =   0xff;
}
