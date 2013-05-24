/*
** graphic.c
**
** Composite Video using:
**      T3      time based
**      OC3     Horizontal Synchronization pulse
**      DMA0    image data 
**      DMA1    back porch extension
**      SPI1 in Frame Slave Mode
**
** Double buffered Graphic page for PIC32MX
**  8/02/07 v2.2 
**  10/23/07 v3.0 72MHz operation
**  11/20/07 v3.1 removed OC3 interrupt
*/

#include <plib.h>
#include <string.h>

#include <explore.h>
#include <graphic.h>
#include <font.h>

// timing for composite video vertical state machine
#ifdef NTSC
#define LINE_N   262        // number of lines in NTSC frame
#define LINE_T   2284       // Tpb clock in a line (63.5us) 
#else
#define LINE_N   312        // number of lines in PAL frame
#define LINE_T   2304       // Tpb clock in a line (64us) 
#endif

// count the number of remaining black lines top+bottom
#define VSYNC_N  3          // V sync lines
#define VBLANK_N (LINE_N -VRES -VSYNC_N)  
#define PREEQ_N  VBLANK_N/2         // preeq + bottom blank 
#define POSTEQ_N VBLANK_N -PREEQ_N  // posteq + top blank 

// definition of the vertical sync state machine
#define SV_PREEQ    0
#define SV_SYNC     1
#define SV_POSTEQ   2
#define SV_LINE     3

// timing for composite video horizontal state machine
#define PIX_T    4          // Tpb clock per pixel 
#define HSYNC_T  180        // Tpb clock width horizontal pulse 
#define BPORCH_T 340        // Tpb clock width back porch 

int VMap1[ VRES*(HRES/32)]; // image buffer 
int *VA = VMap1;            // pointer to the Active VMap
int *VH = VMap1;            // pointer to the Hidden VMap

#ifdef DOUBLE_BUFFER
int VMap2[ VRES*(HRES/32)]; // second image buffer
#endif

volatile int *VPtr;
volatile short VCount;
volatile short VState;

// next state table
short VS[4] = { SV_SYNC, SV_POSTEQ, SV_LINE, SV_PREEQ};
// next counter table
short int VC[4] = { VSYNC_N,  POSTEQ_N,    VRES,  PREEQ_N};

int zero[2]= {0x0, 0x0};


void __ISR(_TIMER_3_VECTOR, ipl7SRS) T3Interrupt( void)
{
//_RA2=1;
    // advance the state machine
    if ( --VCount == 0)
    {
        VCount = VC[ VState&3];
        VState = VS[ VState&3];
    }

    // vertical state machine
    switch ( VState) {
        case SV_SYNC:   // 1
            // vertical sync pulse
            OC3R = LINE_T - HSYNC_T - BPORCH_T;
            break;
            
        case SV_POSTEQ: // 2
          // horizontal sync pulse
            OC3R = HSYNC_T; 
            break;
        
        case SV_PREEQ:  // 0
            // prepare for the new frame
            VPtr = VA;
            break;
            
        default:            
        case SV_LINE:   // 3
            // preload of the SPI waiting for SS (Synch high)
            SPI1BUF = 0;
            // update the DMA0 source address and enable it
            DCH0SSA = KVA_TO_PA((void*) VPtr);
            VPtr += HRES/32;
            DmaChnEnable( 1);
            break;                                               
    } //switch

    // clear the interrupt flag
    mT3ClearIntFlag();
//_RA2=0;    
} // T3Interrupt



void initVideo( void)
{
    // 1. init the SPI1 
    // select framed slave mode to synch SPI with OC3
    SpiChnOpen( 1, SPICON_ON | SPICON_MSTEN | SPICON_MODE32
              | SPICON_FRMEN | SPICON_FRMSYNC | SPICON_FRMPOL 
              , PIX_T);
    
    // 2. make SS1(RB2) a digital input 
    AD1PCFGSET = 0x0004;

    // 3. init OC3 in single pulse, continuous mode 
    OpenOC3( OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 
                0, HSYNC_T);
    
    // 4. Timer3 on, prescaler 1:1, internal clock, period 
    OpenTimer3(  T3_ON | T3_PS_1_1 | T3_SOURCE_INT, LINE_T-1);

    // 5. init the vertical sync state machine
    VState = SV_LINE; 
    VCount = 1;                 
    
    // 6. init the active and hidden screens pointers
    VA = VMap1;
#ifdef DOUBLE_BUFFER
    VH = VMap2;
#else
    VH = VA;
#endif

    // 7. DMA 1 configuration   back porch extension
    DmaChnOpen( 1, 1, DMA_OPEN_DEFAULT);
    DmaChnSetEventControl( 1, DMA_EV_START_IRQ_EN |
                          DMA_EV_START_IRQ(_SPI1_TX_IRQ));
    DmaChnSetTxfer( 1, (void*)zero, (void *)&SPI1BUF, 
                    8, 4, 4);
    
    // 8. DMA 0 configuration  image serialization
    DmaChnOpen( 0, 0, DMA_OPEN_DEFAULT);
    DmaChnSetEventControl( 0, DMA_EV_START_IRQ_EN |
                            DMA_EV_START_IRQ(_SPI1_TX_IRQ));
    DmaChnSetTxfer( 0, (void*)VPtr, (void *)&SPI1BUF,
                    HRES/8, 4, 4);
    // chain DMA0 to completion of DMA1 transfer
    DmaChnSetControl( 0, DMA_CTL_CHAIN_EN | DMA_CTL_CHAIN_DIR);

    // 9. Enable Timer3 Interrupts
    // set the priority level 7 to use shadow register set
    mT3SetIntPriority( 7);
    mT3IntEnable( 1);

} // initVideo


void clearScreen( void)
{ // fill with zeros the Active Video array
    memset( VA, 0, VRES*( HRES/8));

    // reset text cursor position
    cx = cy = 0;

} //clearScreen


void clearHScreen( void)
{ // fill with zeros the Hidden Video array
    memset( VH, 0, VRES*( HRES/8));

    // reset text cursor position
    cx = cy = 0;

} //clearHScreen


void * getAVideo( void)
{ // return a pointer to Active Video array
    return VA;
} // getAVideo


void haltVideo( void)
{
    T3CONbits.TON = 0;   // turn off the vertical state machine
} //haltVideo


void swapV( void)
{
    int * V;

    if ( VState == SV_LINE)
        while ( VCount != 1);       // wait end of the frame
    V = VA; VA = VH; VH = V;        // swap the pointers
    VPtr = VA;                      
} // swapV


void singleV( void)
{ // make all functions work on a single image buffer
    VA = VMap1;
    VH = VA;
} // singleV


void plot( unsigned x, unsigned y) 
{
    if ((x<HRES) && (y<VRES) )
        VH[ ((VRES-1-y)<<3)+(x>>5)] |= ( 0x80000000>>(x&0x1f));
} // plot


#define abs( a)     (((a)> 0) ? (a) : -(a))

void line( short x0, short y0, short x1, short y1)
{
     short steep, t ;
     short deltax, deltay, error;
     short x, y;
     short ystep;

    // simple clipping
     if (( x0 < 0) || (x0>HRES))
        return;
     if (( x1 < 0) || (x1>HRES))
        return;
     if (( y0 < 0) || (y0>VRES))
        return;
     if (( y1 < 0) || (y1>HRES))
        return;

     steep = ( abs(y1 - y0) > abs(x1 - x0));
     
     if ( steep )
     { // swap x and y
         t = x0; x0 = y0; y0 = t;
         t = x1; x1 = y1; y1 = t;
     }
     if (x0 > x1) 
     {  // swap ends
         t = x0; x0 = x1; x1 = t;
         t = y0; y0 = y1; y1 = t;
     }
     
     deltax = x1 - x0;
     deltay = abs(y1 - y0);
     error = 0;
     y = y0;
     
     if (y0 < y1) ystep = 1; else ystep = -1;
     for (x = x0; x < x1; x++)
     {
         if ( steep) plot(y,x); else plot(x,y);
         error += deltay;
         if ( (error<<1) >= deltax)
         {
             y += ystep;
             error -= deltax;
         } // if 
     } // for
} // line 

int cx, cy;

void putcV( char a)
{
    int i, j, *p;
    const char *pf;
    
    // 0. check for backspace
    if ( a == 0x8) 
    {
        if ( cx > 0)
            cx--;
        return;
    }     

    // 1. check if char in range
    if ( a < F_OFFS)         
        return;
    if ( a >= F_OFFS+F_SIZE)   
        return;
     
    // 2. check page boundaries and wrap or scroll as necessary
    if ( cx >= HRES/8)          // wrap around x
    {
        cx = 0;
        cy++;   
    } 
    if ( cy >= VRES/8)          //  scroll up y
    {
        int *pd = VH;
        int *ps = pd + (HRES/32)*8;
        for( i=0; i<(HRES/32)*(VRES-8); i++)
            *pd++ = *ps++;
        for( i=0; i<(HRES/32)*8; i++)
            *pd++ = 0;
        // keep cursor within boundary
        cy = VRES/8-1;
    }    
    // 3. set pointer to word in the video map
    p = &VH[ cy * 8 * HRES/32 + cx/4];
    // set pointer to first row of the character in font array
    pf = &Font8x8[ (a-F_OFFS) << 3];
    
    // 4. copy one by one each line of the character on screen
    for ( i=0; i<8; i++)
    {
        j = (3-(cx & 3))<<3;    // consider MSB first
        *p &= ~(0xff << j);	    // clear background
        *p |= ((*pf++) << j);   // overimposed character

        // point to next row
        p += HRES/32; 
    } // for
    
    // 5. advance cursor position
    cx++;
} // putcV


void putsV( char *s)
{
    while (*s)
        putcV( *s++);
    // advance to next line
    cx=0; cy++;
} // putsV
