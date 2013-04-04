/*
** CONU2.C
**
** Basic CONSOLE I/O for Explorer16 demonstration board
** 
*/

#include <xc.h>
#include "conU2.h"

// I/O definitions for the Explorer16
#define TRTS    TRISFbits.TRISF13   // tris control for RTS pin

// timing and baud rate settings 
#define BRATE    (40000000L /4 /115200) -1      // 115200 baud (BREGH=1)
#define U_ENABLE 0x8008		// enable the UART peripheral
#define U_TX	 0x0400		// enable transmission

// init the serial port UART2, 115200@36MHz, 8, N, 1, CTS/RTS
void initU2( void)
{
	U2BRG 	= BRATE;
	U2MODE 	= U_ENABLE;
	U2STA 	= U_TX;
	TRTS    = 0;        // make RTS output
	RTS     = 1;        // set RTS default status
} // initCon

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
	while ( !U2STAbits.URXDA);	// wait for new char to arrive
	RTS = 1;
	return U2RXREG;		// read the char from receive buffer
}// getU2


// send a null terminated string to the UART2 serial port
int puts( const char *s)
{
	while( *s)			// loop until *s == '\0' end of string
		putU2( *s++);	// send char and point to the next one
    putU2( '\r');       // terminate with a cr / line feed
} // putsU2


char *getsn( char *s, int len)
{
    char *p = s;            // copy the buffer pointer 
    do{
        *s = getU2();       // wait for a new character
        putU2( *s);         // echo character
        
        if (( *s==BACKSPACE)&&( s>p))
        {
            putU2( ' ');     // overwrite the last character
            putU2( BACKSPACE);
            len++;
            s--;            // back the pointer
            continue;
        }
        if ( *s=='\n')      // line feed, ignore it
            continue;
        if ( *s=='\r')      // end of line, end loop
            break;          
        s++;                // increment buffer pointer
        len--;
    } while ( len>1 );      // until buffer full
 
    *s = '\0';              // null terminate the string 
    
    return p;               // return buffer pointer
} // getsn
    
