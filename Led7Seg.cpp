//================================================================
// LED 7 Segment routines
//
// change log:
// 2016-2-5 Initial revision 
//================================================================
#include <Arduino.h>
#include "led7seg.h"

const int led7SegPins[] =
{
  A5, // A 
  A0, // B
  3,  // C
  A3, // D
  A2, // E
  A1, // F
  A4, // G
  2   // DP
};

const int led7SegFonts[] =
{
  // 7 6 5 4 3 2 1 0 - bits positions
  // x A B C D E F G - segments
  0x7E, // 0
  0x30, // 1
  0x6D, // 2
  0x79, // 3
  0x33, // 4
  0x5B, // 5
  0x5F, // 6
  0x70, // 7
  0x7F, // 8
  0x7B  // 9
};

void led7SegInit()
{
  for (unsigned int i = 0; i < (sizeof led7SegPins)/(sizeof led7SegPins[0]); i++)
  {
    pinMode(led7SegPins[i], OUTPUT);
    digitalWrite(led7SegPins[i], HIGH);
  }
}

void led7SegOff()
{
  for (unsigned int i = 0; i < (sizeof led7SegPins)/(sizeof led7SegPins[0]); i++)
  {
    digitalWrite(led7SegPins[i], HIGH);
  }
}

void led7SegDisplayDigit(unsigned int digit)
{
  if (digit <= 9)
  {
     for (unsigned int i = 0; i < 7; i++)
     {
        digitalWrite(led7SegPins[6-i], !((led7SegFonts[digit] >> i) & 0x1));
     }
  }
}

void led7SegDisplayDp()
{
  digitalWrite(led7SegPins[7], LOW);
}

enum led7SegStates
{
  L7S_STATE_DISP_H, // display digit of hundreds
  L7S_STATE_DIM_H,  // dim digit of hundreds
  L7S_STATE_DISP_T, // display digit of tens
  L7S_STATE_DIM_T,  // dim digit of tens
  L7S_STATE_DISP_O, // display digit of ones
  L7S_STATE_DIM_O   // dim digit of ones
};

#define L7S_DISP_DELAY_DIGIT 800
#define L7S_DIM_DELAY_DIGIT  400
#define L7S_DIM_DELAY_NUM    2000


void led7SegDisplayNum(int number)
{
  static int state = L7S_STATE_DISP_H;
  static unsigned long time = 0;
  static int digitH = 0;
  static int digitT = 0;

  if (millis() < time)
  {
    return;  
  }

  if ((number < -999) || (number > 999))
  { 
    return;
  }

  switch (state)
  {
    case L7S_STATE_DISP_H:
      if (number < 100)
      {
        state = L7S_STATE_DISP_T;
      }
      else
      {
        digitH = number/100;
        led7SegDisplayDigit(digitH);
        state = L7S_STATE_DIM_H;
        time = millis() + L7S_DISP_DELAY_DIGIT;
      }
    break;
    case L7S_STATE_DIM_H:
      led7SegOff();
      state = L7S_STATE_DISP_T;
      time = millis() + L7S_DIM_DELAY_DIGIT;
      break;
    case L7S_STATE_DISP_T:
      if (number < 10)
      {
        state = L7S_STATE_DISP_O;
      }
      else
      {
        number = number - digitH*100;
        digitT = number/10;
        led7SegDisplayDigit(digitT);
        state = L7S_STATE_DIM_T;
        time = millis() + L7S_DIM_DELAY_DIGIT;
      }
    break;
    case L7S_STATE_DIM_T:
      led7SegOff();
      state = L7S_STATE_DISP_O;
      time = millis() + L7S_DISP_DELAY_DIGIT;
    break;
    case L7S_STATE_DISP_O:
      number = number - digitH*100 - digitT*10;
      led7SegDisplayDigit(number);
      led7SegDisplayDp();
      state = L7S_STATE_DIM_O;
      time = millis() + L7S_DIM_DELAY_DIGIT;    
    break;
    case L7S_STATE_DIM_O:
    default:
      led7SegOff();
      state = L7S_STATE_DISP_H;
      time = millis() + L7S_DIM_DELAY_NUM;
      break;
  }
}
