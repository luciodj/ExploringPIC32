/*
** SERIAL.C
**
** UART2 RS232 asynchronous communication demonstration code
**
*/
// configuration bit settings, Fcy=80MHz, Fpb=40MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF

#define FCY     80000000L
#define FPB     40000000L

#include <xc.h>

// I/O definitions for the Explorer16
#define CTS     _RF12               // Cleart To Send, input
#define RTS     _RF13               // Request To Send, output
#define TRTS    TRISFbits.TRISF13   // tris control for RTS pin

// timing and baud rate calculations
#define BRATE    (FPB/4/115200)-1   // 115200 baud 
#define U_ENABLE 0x8008	            // enable UART (BREGH=1)
#define U_TX     0x0400	            // enable transmission


// initialize the UART2 serial port
void initU2( void)
{
    U2BRG 	= BRATE;    
    U2MODE 	= U_ENABLE;
    U2STA 	= U_TX;
    TRTS    = 0;        // make RTS output
    RTS     = 1;        // set RTS default status
} // initU2


// send a character to the UART2 serial port
int putU2( int c)
{
    while ( CTS);		        // wait for !CTS, clear to send
    while ( U2STAbits.UTXBF);   // wait while Tx buffer full
    U2TXREG = c;
    return c;
} // putU2


// wait for a new character to arrive to the UART2 serial port
char getU2( void)
{
    RTS = 0;            // assert Request To Send !RTS
    while ( !U2STAbits.URXDA);	// wait for a new character to arrive
    RTS = 1;
    return U2RXREG;		// read the character from the receive buffer
}// getU2


main()
{
    char c;

    // 1. init the UART2 serial port 
    initU2();

    // 2. prompt 
    putU2( '>');
	
    // 3. main loop
    while ( 1)
    {    
        // 3.1 wait for a character
        c = getU2();

        // 3.2 echo the character
        putU2( c);		
	} // main loop
}// main
