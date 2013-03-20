/*
**	ReadTest.c
**
**  07/18/07 v2.0 LDJ  
**  11/23/07 v3.0 LDJ using the LCD display
*/

#include <p32xxxx.h>
#include <plib.h>
#include <explore.h>
#include <SDMMC.h>
#include <LCD.h>
#include "fileio.h"

#define B_SIZE  10
char data[ B_SIZE];


int main( void)
{
    MFILE *fs;
    unsigned r;
    int i, c;
    char s[16];

    //initializations
    initEX16();            
    initLCD();              // init LCD display
 
    // main loop
    while( 1)
    {
        putsLCD( "Insert card...");
        while( !getCD());       // wait for card to be inserted
        Delayms( 100);          // de-bounce
        clrLCD();
    
        if ( mount())
        {
            putsLCD( "mount\n");
            if ( (fs = fopenM( "source.txt", "r")))
            {
                c = 0;
                putsLCD("Reading...");
                do{
                    r = freadM( data, B_SIZE, fs);
                    for( i=0; i<r; i++)
                    {
                        if ( data[ i]=='\n')
                        {
                            c++;
                            sprintf( s, "\n%d lines", c);
                            putsLCD( s);
                        }
                    } // for i
                } while( r==B_SIZE);
                fcloseM( fs);
                homeLCD();
                putsLCD("File closed");
            } 
            else
                putsLCD("File not found!");
    
            unmount();
        } // mounted
        else 
            putsLCD(" Mount Failed!");

        getKEY();
    } // loop    
} // main
