//================================================================
// Quiz slave
//
// change log:
// 2016-2-4  0.1 Initial revision 0.1
// 2016-2-8  0.2 Added slave address config
// 2016-2-17 0.3 Added rx queue clearing after receiving GETREADY message 

//================================================================
#include <EEPROM.h>
#define FW_VER "0.3"

//===============================================================
// RGB LED
//===============================================================
#define RED     255, 0, 0
#define GREEN   0, 255, 0
#define BLUE    0, 0, 255
#define YELLOW  255, 255, 0 
#define PURPLE  255, 0, 255   
#define ACQUA   0, 255, 255   
#define OFF     0, 0, 0   

enum glow_states
{
  GLOW_GREEN,
  GLOW_BLUE,
  GLOW_PURPLE,
  GLOW_RED,
  GLOW_YELLOW
};

//uncomment this line if using a Common Anode LED
//#define COMMON_ANODE
const int redPin = 8;
const int greenPin = 7;
const int bluePin = 6;

void setColor(uint8_t red, uint8_t green, uint8_t blue)
{
  // RED, GREEN 127-128, BLUE 0-105
#ifdef COMMON_ANODE
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
#endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

//===============================================================
// LED 7 segment display
//===============================================================
#include "led7seg.h"

//================================================================
// Sound
//================================================================

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 18 5
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

const int buzzPin = 5;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void playSound(const int buzzPin) {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(buzzPin);
  }
}

//================================================================
// Button
//================================================================
const int buttonPin = 4;
const int debounceDelay = 20; //20 ms
const int longPress = 500;
int buttonState = HIGH;
int buttonLongState = HIGH;
int buttonStateChanged = false;
void scanButton()
{
  static long lastDebounceTime = 0;
  static int lastButtonState = HIGH;
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);
  
 // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) 
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
 
  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) 
    {
      buttonState = reading;
      if (buttonState == HIGH)
      {
         buttonLongState = HIGH;
      }
   
      buttonStateChanged = true;
    }
  }

  if (((millis() - lastDebounceTime) > longPress) && (buttonState == LOW)) 
  {
    buttonLongState = LOW;
  }
  
  lastButtonState = reading;
}

int buttonPressed()
{
  if  ((buttonStateChanged == false) || (buttonState == HIGH))
  {
    return false;
  }
  else if (buttonStateChanged == true)
  {
    buttonStateChanged = false;
    return true;
  }
  else
  {
    return false;   
  }
}


//================================================================
// Radio RF24
//================================================================  
#include <SPI.h>
#include "RF24.h"
#include "printf.h" //required for RF24 radio.printDetails()

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 9 (CE) & 10 (CS)  */
RF24 radio(9, 10);
#define MAX_SLAVES 6

enum radio_msgs           // message format 
{                         //| byte 0 |  byte 1 |  byte 3 |
  RADIO_TEST,             //|  00h   | XXXXXXX | XXXXXXX |
  RADIO_GETREADY,         //|  01h   | XXXXXXX | XXXXXXX |
  RADIO_PRESSED,          //|  02h   | player  | XXXXXXX |
  RADIO_FIRST,            //|  03h   | player  |  score  |
  RADIO_WRONG,            //|  04h   | player  | XXXXXXX |
  RADIO_RIGHT             //|  04h   | player  | XXXXXXX |
};

uint8_t addresses[][6] = {"0Node", "1Node", "2Node","3Node","4Node","5Node","6Node"};

void radioEmptyRxQueues()
{
  uint8_t data[5];
  while (radio.available())
    radio.read(data,sizeof(data)); //read remaining data in buffers to clean it up
}

//================================================================
// Setup
//================================================================  
enum slave_states
{
  SLAVE_INIT,
  SLAVE_CONFIG,
  SLAVE_IDLE,
  SLAVE_WAIT,
  SLAVE_PRESSED,
  SLAVE_FIRST,
  SLAVE_RIGHT,
  SLAVE_WRONG,
};

static int state = SLAVE_IDLE;
static int correctAnswers = 0;

uint8_t slaveNumber = 0;
#define MAX_SLAVES 6

void setup()
{
  Serial.begin(115200);
  Serial.print("Quiz Slave, FW.V.:");
  Serial.print(FW_VER);
  Serial.println(" (C) 2016 Candeo Tech");
  led7SegInit();
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  tone(buzzPin, NOTE_C3, 400);  
 
  printf_begin();

  // RF24 init
  Serial.println("RF24 init");
  radio.begin();    
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(105);
  radio.setDataRate(RF24_250KBPS);
  radio.printDetails();
  radio.stopListening();
  radio.openReadingPipe(1,addresses[0]);
  radio.startListening();                       // Start listening  

  slaveNumber = EEPROM.read(0);
  if (slaveNumber >= MAX_SLAVES)
    slaveNumber = 0;

  if (digitalRead(buttonPin) == LOW)
  {
    delay(500);
    tone(buzzPin, NOTE_C3, 400);
    state = SLAVE_CONFIG;
  }
  else
  {
    led7SegDisplayDigit(slaveNumber);
    delay(1000);
    state = SLAVE_IDLE;     
  }
}

//================================================================
// Main loop
//================================================================  


//----------------------------------------------------------------
// CONFIG state
//----------------------------------------------------------------  
void mConfig()
{
   
   led7SegDisplayDigit(slaveNumber);

    if (buttonPressed())
    {
      slaveNumber++;
      
      if (slaveNumber >= MAX_SLAVES)
        slaveNumber = 0;
    }
   
    if (buttonLongState == LOW)
    {
      EEPROM.write(0, slaveNumber);
      state = SLAVE_IDLE;
    }
}


//----------------------------------------------------------------
// IDLE state
//----------------------------------------------------------------  
void mIdle()
{
  uint8_t data[3], pipeNum;

  setColor(OFF);
  
  if(radio.available(&pipeNum))
  {
    radio.read(data,sizeof(data));
    if (data[0] == RADIO_GETREADY)
    {
   delay(100); //add time to Rx repeated message
      radioEmptyRxQueues();
      state = SLAVE_WAIT;
    }
  }
}

//----------------------------------------------------------------
// WAIT state
//----------------------------------------------------------------  
void mWait()
{
  uint8_t data[] = {RADIO_PRESSED, slaveNumber};
  
  setColor(ACQUA); 
  //`rgbGlow();
  radio.stopListening();  
  if (buttonPressed())
  {
    radio.openWritingPipe(addresses[slaveNumber]);
    for (int i = 0; i < 3; i++) 
    {
      //looks like switch produces noise, to be sure we send 3 messages  
        radio.write(&data, sizeof(data));
    }
    tone(buzzPin, NOTE_C2, 200);
    radio.startListening();
    state = SLAVE_PRESSED;
  }
}

//----------------------------------------------------------------
// PRESSED state
//----------------------------------------------------------------  
void mPressed()
{
  uint8_t data[3];
  setColor(PURPLE); 
 
  if(radio.available())
  {
    radio.read(data,sizeof(data));
    if (data[0] == RADIO_GETREADY)
    {
      state = SLAVE_WAIT;
    }
    else if ((data[0] == RADIO_FIRST) && (data[1] == slaveNumber))
    {
      Serial.println("FIRST");
      setColor(YELLOW);
      delay(100);       //add time to Rx repeated message
      radioEmptyRxQueues();

      state = SLAVE_FIRST;
    }
  }    
}

//----------------------------------------------------------------
// FIRST state
//----------------------------------------------------------------  
void mFirst()
{ 
  uint8_t data[3]; 
 
  if(radio.available())
  { 
    radio.read(data,sizeof(data));
    Serial.println(data[1]);
    if ((data[0] == RADIO_RIGHT) && (data[1] == slaveNumber))
    {
      
      // add score compare
      state = SLAVE_RIGHT;
    }
    else if ((data[0] == RADIO_WRONG) && (data[1] == slaveNumber))
    {
      state = SLAVE_WRONG;
    }
    else if (data[0] == RADIO_GETREADY)
    {
      state = SLAVE_WAIT;
    }
  }    
}

//----------------------------------------------------------------
// RIGHT state
//----------------------------------------------------------------  
void mRight()
{
  setColor(GREEN); 
  correctAnswers++; 
  playSound(buzzPin);
  state = SLAVE_IDLE;
}

//----------------------------------------------------------------
// WRONG state
//----------------------------------------------------------------  
void mWrong()
{
  setColor(RED); 
  tone(buzzPin, NOTE_C1, 400);
  delay(1000);
  tone(buzzPin, NOTE_B0, 400);
  delay(1000);
  state = SLAVE_IDLE;
}

void loop()
{
  if (state != SLAVE_CONFIG)
    led7SegDisplayNum(correctAnswers);
  
  scanButton();

  switch (state)
  {
     case SLAVE_CONFIG:
      mConfig();
      break;
     case SLAVE_IDLE:
      mIdle();
      break;
    case SLAVE_WAIT:
      mWait();
      break;
    case SLAVE_PRESSED:
      mPressed();
      break;
    case SLAVE_FIRST:
      mFirst();
      break;
    case SLAVE_RIGHT:
      mRight();
      break;
    case SLAVE_WRONG:
      mWrong();
      break;
    default:
      break;
  }
}


