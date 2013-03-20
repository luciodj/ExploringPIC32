/*
** Strings
*/

#include <p32xxxx.h>
#include <string.h>

// 1. variable declarations
//const char e[5] = { 'H','E', 'L','L','O'};
//char f[] = "HELLO";
const char a[] = "Exploring the PIC32";
char b[100] = "Initialized";

// 2. main program
main()
{
    strcpy( b, "MPLAB C32");	// assign new content to b
} // main
