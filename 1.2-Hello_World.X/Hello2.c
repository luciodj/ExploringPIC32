/*
** Hello Embedded World
** Hello2.c
*/

#include <xc.h>

main()
{
    // disable the JTAG port
    DDPCONbits.JTAGEN = 0;
    
    // configure all PORTA pins as output
    TRISA =   0;        // all PORTA as output
    PORTA =   0xff;
}
