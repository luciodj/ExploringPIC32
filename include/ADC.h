/*
** ADC.h
**
*/
#define POT     5       // 10k potentiometer on AN5 input
#define TSENS   4       // TC1047 Temperature sensor on AN4
#define AINPUTS 0xffcf  // Analog inputs for POT and TSENS

// initialize the ADC for single conversion, select input pins
void initADC( int amask) ;
int readADC( int ch);
