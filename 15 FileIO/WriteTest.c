/*
**	WriteTest.c
**
**  11/25/07 v1.1 LDJ PIC32, uses LCD
*/
#include <p32xxxx.h>
#include <explore.h>
#include <LCD.h>
#include <SDMMC.h>
#include "fileio.h"

#define B_SIZE   100

char data[B_SIZE];


int main( void)
{
    MFILE *fs, *fd;
    unsigned c, i, p, r;
    char s[32];

    //initializations
    initEX16();
    initLCD();              // init LCD display
 
    putsLCD( "Insert card...\n");
    while( !getCD());    // wait for card to be inserted
    Delayms( 100);          // wait for card to power up
        
    if ( mount())
    {
        clrLCD();
        if ( (fs = fopenM( "source.txt", "r")))
        {
            if ( (fd = fopenM( "dest.txt", "w")))
            {
                c = 0;              // init byte counter
                p = 0;              // init progress index
                i = fs->size/16;    // progress bar increment
                
                putsLCD("Copying\n");
                do{
                    // copy data
                    r = freadM( data, B_SIZE, fs);
                    r = fwriteM( data, r, fd);
                    
                    // update progress bar
                    c += r;
                    while (p < c/i)
                    {
                        p++;
                        putLCD( 0xff);  // add one bar    
                    }
                } while( r == B_SIZE);

                r = fcloseM( fd);
                if ( r == TRUE)
                {
                    clrLCD();
                    sprintf( s, "Copied \n%d bytes", c);
                    putsLCD( s);
                } // close dest
                else
                    putsLCD("ER:closing dest");
            } // open dest
            else 
                putsLCD("ER:creating file");

            fcloseM( fs);
        } // open source 
        else
            putsLCD("ER:open source");

        unmount();
    } // mount
    else 
        putsLCD("ER:mount failed");
    
    // main loop
    while( 1);
} // main

