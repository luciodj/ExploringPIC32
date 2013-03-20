/*
** CONU2.h
** console I/O library for Explorer16 board
** v 2.0 7/18/07 LDJ 
*/

// I/O definitions for the Explorer16
#define CTS 	_RF12   // Cleart To Send, input, HW handshake
#define RTS     _RF13   // Request To Send, output, HW handshake
#define BACKSPACE 0x8   // ASCII backspace character code            

// init the serial port (UART2, 115200 baud, 8, N, 1, CTS/RTS )
void initU2( void);

// send a character to the serial port
int putU2( int c);

// wait for a new character to arrive to the serial port
char getU2( void);

// send a null terminated string to the serial port
int puts( const char *s);

// receive a null terminated string in a buffer of len char
char * getsn( char *s, int n);

// useful macros
#define clrscr() puts( "\x1b[2J") 
#define home()   puts( "\x1b[1,1H") 
