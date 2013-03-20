/*
** AudioPWM.c
**
** 07/22/06 v1.1 LDJ  PIC24
** 07/14/07 v2.0 LDJ PIC32 porting
** 11/23/07 V2.1 LDJ merging wav and audio PWM
*/

#include <p32xxxx.h>
#include <plib.h>
#include <stdlib.h>
#include <explore.h>
#include <sdmmc.h>
#include <fileio.h>
#include <LCD.h>

#define B_SIZE  512         // audio buffer size

// audio configuration 
typedef struct {
    char stereo;            // 0 - mono  1- stereo
    char fix;               // sign fix 0x00 8-bit, 0x80 16-bit
    char skip;              // advance pointer to next sample
    char size;              // sample size (8 or 16-bit)
} AudioCfg;

// chunk IDs
#define  RIFF_DWORD  0x46464952UL
#define  WAVE_DWORD  0x45564157UL
#define  DATA_DWORD  0x61746164UL
#define  FMT_DWORD   0x20746d66UL     
#define  WAV_DWORD   0x00564157UL

typedef struct {
    // data chunk
    unsigned int dlength;       // actual data size
    char    data[4];            // "data"

    // format chunk 
    unsigned short  bitpsample; // bit per sample
    unsigned short  bpsample;   // bytes per sample
                                //    (4=16bit stereo)
    unsigned int    bps;        // bytes per second
    unsigned int    srate;      // sample rate in Hz
    unsigned short  channels;   // # of channels
                                //  (1= mono,2= stereo)
    unsigned short  subtype;    // always 01
    unsigned int    flength;    // size of this block (16)
    char    fmt_[4];            // "fmt_"
    
    char    type[4];            // file type name "WAVE"
    unsigned int    tlength;    // size of encapsulated block
    char    riff[4];            // envelope "RIFF"
} WAVE; 

// global definitions
char    ABuffer[ 2][ B_SIZE];   // double data buffer
volatile char  CurBuf;          // index of buffer in use
volatile char  AEmptyFlag;      // buffer needs to be filled

// audio configuration parameters
AudioCfg ACfg;              

// local definitions
unsigned char *BPtr;            // pointer inside active buffer
short BCount;                   // count bytes used 


void initAudio( void)
{ // configures peripherals for Audio playback 
    
    // 1. activate the PWM modules 
    // CH1 and CH2 in PWM mode, TMR2 based
    OpenOC1( OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE,
            0, 0);
    OpenOC2( OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE,
            0, 0);

    // 2. init the timebase
    // enable TMR2, prescale 1:1, internal clock, period
    OpenTimer2(T2_ON | T2_PS_1_1 |  T2_SOURCE_INT, 0);         
    mT2SetIntPriority( 4);      // set TMR2 interrupt priority
    
} // initAudio


void startAudio( int bitrate, int position, int count)
{ // begins the audio playback
    
    // 1. init pointers  and flags
    CurBuf = 0;                 // buffer 0 active first
    BPtr = ABuffer[ CurBuf] + position;
    AEmptyFlag = FALSE;

    // 2. number of actual samples to be played
    BCount = count/ACfg.skip; 

    // 3. set the period for the given bitrate
    PR2 = FPB / bitrate-1;    
    
    // 4. enable the interrupt state machine
    mT2ClearIntFlag();             // clear interrupt flag
    mT2IntEnable( 1);              // enable TMR2 interrupt
} // startAudio


void haltAudio( void)
{ // stops playback state machine
   mT2IntEnable( 0);
} // halt audio


void __ISR( _TIMER_2_VECTOR, ipl4 ) T2Interrupt( void)
{
    // 0. allow interrupt nesting
    asm( "ei");
    
    // 1. load the new samples for the next cycle
    OC1RS = 30+(*BPtr ^ ACfg.fix); 
    if ( ACfg.stereo)
        OC2RS = 30 + (*(BPtr + ACfg.size) ^ ACfg.fix);
    else    // mono
        OC2RS = OC1RS;

    // 2. skip samples to reduce the bitrate
    BPtr += ACfg.skip;

    // 3. check if buffer emptied
    if ( --BCount == 0)
    {
        // 3.1 swap buffers
        CurBuf = 1- CurBuf;
        
        // 3.2. place pointer on first sample
        BPtr = &ABuffer[ CurBuf][ACfg.size-1];
        
        // 3.3 restart counter
        BCount = B_SIZE/ACfg.skip;

        // 3.4 flag a new buffer needs to be filled
        AEmptyFlag = 1;
    }

    // 4. clear interrupt flag and exit
    mT2ClearIntFlag();
} // T2Interrupt


/*-------------------------------------------------------------
** Wave
** 
** 	Wave File Player 
**	Uses 2 x 8 bit PWM channels at 44kHz
**
** revision:
** 05/02/03 v1.00 LDJ PIC18
** 07/08/06 v2.00 LDJ porting to PIC24/C30
** 07/14/07 v3.00 LDJ PIC32 porting
*/
int playWAV( char *name)
{
    WAVE    wav;
    MFILE    *f;
    unsigned int lc, r;
    int wi, pos, rate, period, last;
    char s[16];
        
    // 1. open the file           
    if ( (f = fopenM( name, "r")) == NULL)
    {   // failed to open 
        return FALSE;
    }
    
    // 2. verify it is a RIFF formatted file
    if ( ReadL( f->buffer, 0) != RIFF_DWORD)
    {
        fcloseM( f);
        return FALSE;
    }
    
    // 3. look for the WAVE chunk signature 
    if ( (ReadL( f->buffer, 8)) != WAVE_DWORD)
    {
        fcloseM( f);
        return FALSE;
    }
    
    // 4. look for the chunk containing the wave format data
    if ( ReadL( f->buffer, 12) != FMT_DWORD)
    {
        fcloseM( f);
        return FALSE;
    }
    
    wav.channels    = ReadW( f->buffer, 22);
    wav.bitpsample  = ReadW( f->buffer, 34);
    wav.srate       = ReadL( f->buffer, 24);
    wav.bps         = ReadL( f->buffer, 28);
    wav.bpsample    = ReadW( f->buffer, 32);
    
    // 5. search for the data chunk
    wi = 20 + ReadW( f->buffer, 16);  
    while ( wi < 512)
    {
        if (ReadL( f->buffer, wi) == DATA_DWORD)
            break;
        wi += 8 + ReadW( f->buffer, wi+4);
    }
    if ( wi >= 512) // could not find in current sector
    {
        fcloseM( f);
        return FALSE;
    }   

    // 6. find the data size (actual wave content)
    wav.dlength = ReadL( f->buffer, wi+4);
    
    // 7. if sample rate too high, skip
    rate = wav.bps / wav.bpsample; // rate = samples per second
    ACfg.skip = wav.bpsample;      // skip to reduce bandwith 
    while ( rate > 48000)
    {
        rate >>= 1;                // divide sample rate by two
        ACfg.skip <<= 1;           // multiply skip by two
    }

    // 8. check if sample rate too low
    period = (FPB/rate)-1;
    if ( period > ( 65536L))       // max timer period 16 bit
    {   // period too long
        fcloseM( f);
        return FALSE;
    }

    // 9. init the Audio state machine
    CurBuf = 0;
    pos   = wi+8;                   // data begin 
    ACfg.stereo = (wav.channels == 2);
    ACfg.size  = 1;                 // #bytes per channel
    ACfg.fix   = 0;                 // sign fix / 16 bit file
    if ( wav.bitpsample == 16)
    {                               // if 16-bit 
        pos++;                      // add 1 to get the MSB
        ACfg.size = 2;              // two bytes per sample
        ACfg.fix  = 0x80;           // fix the sign
    }
    
    // 10 # of bytes composing the wav data chunk
    lc = wav.dlength;
    
    // 11. pre-load both buffer
    r = freadM( ABuffer[0], B_SIZE*2, f);
    lc -= r;                    
    AEmptyFlag = FALSE;
        
    // 12. configure Player state machine and start
    initAudio();
    startAudio( rate, pos, r-pos);
        
    // 13. keep feeding the buffers in the playing loop
    //     as long as entire buffers can be filled
    while (lc > 0)            
    {   // 13.1 check user input to stop playback
        if ( readKEY())             // if any button pressed
        {                           
            lc = 0;                 // playback completed
            break;
        }
        
        // 13.2 check if a buffer needs a refill
        if ( AEmptyFlag)
        {
            r = freadM( ABuffer[1-CurBuf], B_SIZE, f);
            lc-= r;                 // decrement byte count
            AEmptyFlag = FALSE;     // refilled

            // <<put here additional tasks>>
            putsLCD("\n");          //  on the second line
            sprintf( s, "%dKB", (wav.dlength-lc)/1024);
            putsLCD( s);            // byte count
        }
    } // while wav data available

    // 14. pad the rest of the buffer 
    last = ABuffer[1-CurBuf][r-1];
    while( r<B_SIZE) 
        ABuffer[1-CurBuf][r++] = last;
    AEmptyFlag = FALSE;         // refilled

    // 15.finish the last buffer
    AEmptyFlag = FALSE;
    while (!AEmptyFlag);
    
    // 16. stop playback 
    haltAudio();

    // 17. close the file 
    fcloseM( f);

    // 18. return with success
    return TRUE;

} // playWAV
         


