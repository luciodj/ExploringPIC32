/*
** AudioPWM.c
**
*/

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
    char stride;            // number of bytes per period (stereo * size)
} AudioCfg;

// chunk IDs
#define  RIFF_DWORD  0x46464952UL
#define  WAVE_DWORD  0x45564157UL
#define  DATA_DWORD  0x61746164UL
#define  FMT_DWORD   0x20746d66UL     
#define  WAV_DWORD   0x00564157UL

typedef struct {                // define a basic "chunk"
    long ckID;
    long ckSize;
    long ckType;
} chunk;

typedef struct {                // define the WAV format chunk
    unsigned short subtype;     // compression code
    unsigned short channels;    // # of channels (1=mono, 2=stereo)
    unsigned long srate;        // sample rate in Hz
    unsigned long bps;          // bytes per second
    unsigned short align;       // block alignment
    unsigned short bitpsample;  // bit per sample
    // unsigned short extra     // extra format bytes
} WAVE_fmt;


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


void startAudio( int bitrate)
{ // begins the audio playback
    
    // 1. init pointers  and flags
    CurBuf = 0;                 // buffer 0 active first
    BPtr = &ABuffer[ CurBuf][ ACfg.size-1];
    AEmptyFlag = FALSE;

    // 2. number of actual samples to be played
    BCount = B_SIZE/ACfg.skip;

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
    static int sk = 1;

    // 1. skip to increase the bitrate( avoid PWM noise)
    if ( --sk == 0)
    {
        // reload the skip
        sk = ACfg.skip;

        // 2. load the new samples for the next cycle
        OC1RS = 30+(*BPtr ^ ACfg.fix);
        if ( ACfg.stereo == 2)
            OC2RS = 30 + (*(BPtr + ACfg.size) ^ ACfg.fix);
        else    // mono
            OC2RS = OC1RS;

        // 2. advance pointer
        BPtr += ACfg.stride;

        // 3. check if buffer emptied
        BCount -= ACfg.stride;
        if ( BCount <= 0)
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
    } // if skip

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
    chunk       ck;
    WAVE_fmt    wav;
    MFILE       *fp;
    unsigned long lc, r;

    // audio codec parameters
    int period, rate, last;
    char s[16];
        
    // 1. open the file           
    if ( (fp = fopenM( name, "r")) == NULL)
    {   // failed to open 
        return FALSE;
    }
    
    // 2. verify it is a RIFF-WAVE formatted file
    freadM( (void*)&ck, sizeof( chunk), fp);
    if ( (ck.ckID != RIFF_DWORD) || ( ck.ckType != WAVE_DWORD))
        goto Exit;

    
    // 3. look for the chunk containing the wave format data
    freadM( (void*)&ck, 8, fp);
    if ( ck.ckID != FMT_DWORD)
        goto Exit;
    
    // 4. get the WAVE_fmt data
    freadM( (void*) &wav, sizeof( WAVE_fmt), fp);

    // 5. skip extra format bytes
    fseekM( fp, ck.ckSize - sizeof( WAVE_fmt));

    // 6. search for the data chunk
    while( 1)
    {   // read next chunk
        if ( freadM( (void*)&ck, 8, fp) != 8)    // failed, eof
            goto Exit;

        // if not a data chunk, skip
        if ( ck.ckID == DATA_DWORD)
            break;

        fseekM( fp, ck.ckSize);
    }
    
    // 7. find the data size (actual wave content)
    lc = ck.ckSize;
    if (lc < B_SIZE*2)              // accept only files longer than 2* BSIZE
        goto    Exit;
    
    // 8. compute the period and adjust the bit rate to reduce noise
    rate = wav.srate;               // rate = samples per second
    ACfg.skip = 1;                  // skip to reduce bandwith
    while ( rate < 22050)
    {
        rate <<= 1;                // multiply sample rate by two
        ACfg.skip >>= 1;           // multiply skip by two
    }

    // 9. check if sample rate is compatible with the Timer prescaler
    period = (FPB/rate)-1;
    if ( period > ( 65536L))       // max timer period 16 bit
        goto Exit;

    // 10. init the Audio state machine
    CurBuf = 0;
    ACfg.stereo = wav.channels;
    ACfg.size  = 1;                 // #bytes per channel
    ACfg.fix   = 0;                 // sign fix / 16 bit file
    if ( wav.bitpsample == 16)
    {                               // if 16-bit 
        ACfg.size = 2;              // two bytes per sample
        ACfg.fix  = 0x80;           // fix the sign
    }
    ACfg.stride = ACfg.size * ACfg.stereo;

    // 11. pre-load both buffer
    r = freadM( ABuffer[0], B_SIZE*2, fp);
    lc -= r;                    
    AEmptyFlag = FALSE;
        
    // 12. configure Player state machine and start
    initAudio();
    startAudio( rate);
        
    // 13. keep feeding the buffers in the playing loop
    //     as long as entire buffers can be filled
    while (lc >= B_SIZE)
    {   // 13.1 check user input to stop playback
        if ( readKEY())                 // if any button pressed
        {                           
            lc = 0;                     // playback completed
            break;
        }
        
        // 13.2 check if a buffer needs a refill
        if ( AEmptyFlag)
        {
            r = freadM( ABuffer[1-CurBuf], B_SIZE, fp);
            if ( r < B_SIZE)
                goto Exit;              // eof?

            AEmptyFlag = FALSE;         // refilled
            lc -= r;                    // decrement byte count

            // <<put here additional tasks>>
            putsLCD("\n");          //  on the second line
            sprintf( s, "-%dKB ", lc/1024);
            putsLCD( s);            // byte count
        }
    } // while wav data available

    // 14. flash the buffer with data tail
    if ( lc>0)
    {   // load the last sector
        r = freadM( ABuffer[1-CurBuf], lc, fp);

        last = ABuffer[1-CurBuf][r-1];
        while( ( r<B_SIZE) && ( last>0))
            ABuffer[1-CurBuf][r++] = last;
        AEmptyFlag = FALSE;         // refilled

        // wait for current buffer to be emptied
        AEmptyFlag = FALSE;
        while (!AEmptyFlag);
    }

    // 15. finish the last buffer
    AEmptyFlag = 0;
    while (!AEmptyFlag);

Exit:
    // 16. stop playback 
    haltAudio();

    // 17. close the file 
    fcloseM( fp);

    // 18. return with success
    return TRUE;

} // playWAV
         


