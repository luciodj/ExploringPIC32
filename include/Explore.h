/*
** Explore.h
** 
** definitions and prototypes for use with the Explorer16 board
*/

#include <plib.h>

#define FALSE   0
#define TRUE    !FALSE
#define FCY     80000000L
#define FPB     40000000L

#define POT     5       // 10k potentiometer on AN5 input
#define TSENS   4       // TC1047 Temperature sensor on AN4
#define AINPUTS 0xffcf  // Analog inputs for POT and TSENS


// uncomment the following line if using the PIC32 Starter Kit
//#define PIC32_STARTER_KIT

// function prototypes

/* 
** System initialization for max performance and use of the 
** Explorer 16 features (LCD, LED, buttons, timers, SEE)
*/
void initEX16( void);

/*
**
**	Buttons read and debounce
*/

int getKEY();

int readKEY();

/*
** Simple delay routine using TMR1
**
*/

void Delayms( unsigned);

