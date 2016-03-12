#ifndef LED_7_SEG_H
#define LED_7_SEG_H
//================================================================
// LED 7 Segment routines
//
// change log:
// 2016-2-5 Initial revision 
//================================================================
void led7SegInit();
void led7SegOff();
void led7SegDisplayDigit(unsigned int digit);
void led7SegDisplayNum(int number);

#endif //#ifndef LED_7_SEG_H
