/*
** RTCC Interrupts
**
** example 7
*/

#include <plib.h>

int dSec = 0;
int Sec = 0;
int Min = 0;

// 1. Timer1 interrupt service routine
void __ISR_SINGLE__ RTCCInterrupt( void)
{	
    // 1.1  your code here, will be executed only once a year 
    // or once every 365 x 24 x 60 x 60 x 16,000,000 MCU cycles
    // that is once every 504,576,000,000,000 MCU cycles
    

    // 1.2 clear the interrupt flag
    mRTCCClearIntFlag();
} //RTCCInterrupt

main()
{
    // 2.1 init I/Os
    DDPCONbits.JTAGEN = 0;  // disable JTAG port
    TRISA = 0xff00;         // set PORTA LSB as output

    // 2.2 configure RTCC module
    RtccInit();             // inits the RTCC
    // set present time
    rtccTime tm; tm.sec=0x15; tm.min=0x30; tm.hour=01; 
    // set present date
    rtccDate dt; 
    dt.wday=0; dt.mday=0x15; dt.mon=0x10; dt.year=0x07;
    RtccSetTimeDate(tm.l, dt.l);
    // set desired alarm to Feb 29th
    dt.wday=0; dt.mday=0x29; dt.mon=0x2; 
    RtccSetAlarmTimeDate(tm.l, dt.l); 

    // 2.2 init interrupts, 
    mRTCCSetIntPriority( 1);
    mRTCCClearIntFlag();	    
    INTEnableSystemSingleVectoredInt();         
    mRTCCIntEnable( 1);

    // 2.3. main loop
    while( 1)
    {
        // your main code here
        // ...     	
    } // main loop
} // main
