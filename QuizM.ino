//================================================================
// Quiz master
//
// change log:
// 2016-2-1  0.1 Initial revision
// 2016-2-17 0.2 Added sending 3x GETREADY message
//================================================================
#define FW_VER "0.2"

//================================================================
// Utils
//================================================================
#define TASK_READY(time) {if (millis() < (time)) return;}
#define TASK_SCHEDULE(delay) (millis() + delay)
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

const int buzzPin = 3;

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
// LEDs
//================================================================
#define LED_GREEN 0
#define LED_RED   1
#define LED_BOTH  2

const int ledRedPin = 7;  
const int ledGreenPin = 5;

void ledInit()
{
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
}

void ledOn(int led)
{
  switch (led)
  {
    case LED_GREEN:
      digitalWrite(ledGreenPin, HIGH); 
      break;
    case LED_RED:
      digitalWrite(ledRedPin, HIGH); 
      break;
    case LED_BOTH:
       digitalWrite(ledGreenPin, HIGH); 
       digitalWrite(ledRedPin, HIGH); 
    break;
    default:
    break; 
  }
}

void ledOff(int led)
{
  switch (led)
  {
    case LED_GREEN:
      digitalWrite(ledGreenPin, LOW); 
      break;
    case LED_RED:
      digitalWrite(ledRedPin, LOW); 
      break;
    case LED_BOTH:
       digitalWrite(ledGreenPin, LOW); 
       digitalWrite(ledRedPin, LOW); 
    break;
    default:
    break; 
  }
}

#define LED_BLINK_DELAY 500

void ledBlink(int led)
{
  static unsigned long time = 0;
  static int state = false;

  TASK_READY(time);

  if (state == true)
  {
    ledOn(led);
  }
  else
  {
    ledOff(led);
  }
  state = state ? false : true;
  time = TASK_SCHEDULE(LED_BLINK_DELAY);       
}

//================================================================
// Button
//================================================================
const int buttonRedPin = 8;  
const int buttonGreenPin = 6;
const int debounceDelay = 20; //20 ms

#define BTN_NONE  0
#define BTN_RED   1
#define BTN_GREEN 2
#define BTN_BOTH  3

int buttonState = BTN_NONE;
int buttonStateChanged = false;

void buttonInit()
{
  pinMode(buttonRedPin, INPUT_PULLUP);
  pinMode(buttonGreenPin, INPUT_PULLUP);
}

void buttonScan()
{
  static long lastDebounceTime = 0;
  static int lastButtonState = HIGH;
  // read the state of the switch into a local variable:
  int reading = (!(digitalRead(buttonRedPin)) & 0x01) | (!(digitalRead(buttonGreenPin)) & 0x01) << 1;
  
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
      
      buttonStateChanged = true;
    }
  }
    
  lastButtonState = reading;
}

int getButton()
{
  if  (buttonStateChanged == false)
  {
    return BTN_NONE;
  }
  else 
  {
    buttonStateChanged = false;
    return buttonState;
  }
}

//================================================================
// LCD
//================================================================  
#include <LiquidCrystal.h>

LiquidCrystal lcd(A0, A1, A2, A3, A5, A4, 2);

//================================================================
// Radio RF24u
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
void setup()
{
  buttonInit();
  ledInit();
  
  Serial.begin(115200);
  printf_begin();

  // RF24 init
  Serial.println("RF24 init");
  radio.begin(); 
  radio.setPALevel(RF24_PA_MAX);
  radio.printDetails();
  radio.stopListening();
    
  lcd.begin(16, 2);
  lcd.print("Quiz Master");  
  lcd.setCursor(0, 1);
  lcd.print("FW ver.: ");  
  lcd.print(FW_VER);
  delay(1000);
  tone(buzzPin, NOTE_C3, 400);  
}

//================================================================
// Main loop
//================================================================  

#define MAX_PLAYERS 6

unsigned int score[MAX_PLAYERS+1]={0,}; //index starting from 1
unsigned int firstPlayer = 0;

enum master_states
{
  MASTER_INIT,
  MASTER_CONFIG,
  MASTER_IDLE,
  MASTER_WAIT,
  MASTER_ASSESS,
  MASTER_RIGHT,
  MASTER_WRONG,
};

static int state = MASTER_IDLE;

void mInit()
{
  
}

void mConfig()
{
  
}

void displayScore()
{
#define SCORE_DELAY 1500
  static int state = 0;

  static unsigned long time = 0;
  TASK_READY(time);

  lcd.clear();
  lcd.print("Players:");
  lcd.print("   ");
  lcd.print(2*state+1);
  lcd.print("   ");
  lcd.print(2*state+2);
  lcd.setCursor(0, 1);
  lcd.print("Score:");
  lcd.print("     ");
  lcd.print(score[2*state+1]);
  lcd.print("   ");
  lcd.print(score[2*state+2]);
  
  state = (state+1) % 3;
  
  time = TASK_SCHEDULE(SCORE_DELAY);       
}

void mIdle2Wait()
{
  uint8_t data = RADIO_GETREADY;
  
  lcd.clear();
  lcd.print("Waiting...");
  ledOff(LED_GREEN);
  
  radio.stopListening();
  radio.openWritingPipe(addresses[0]);
  for(int i = 0; i < 3; i++)
    radio.write(&data, sizeof(data));
  
  for (int i = 0; i < MAX_SLAVES; i++)
  {   
    radio.openReadingPipe(i,addresses[i+1]);
  }    
  radio.startListening();
  state = MASTER_WAIT; 
}

void mIdle()
{
  ledBlink(LED_GREEN);
  displayScore();
  if (getButton() == BTN_GREEN)
  {
    mIdle2Wait();
  }
}

void mWait()
{
  uint8_t pipeNum;
  uint8_t data[2];
 
  ledBlink(LED_RED);
   
  if(radio.available(&pipeNum))
  {
    while (radio.available())
      radio.read(data,sizeof(data));
    
    if (data[0] == RADIO_PRESSED)  
      firstPlayer = data[1];
    lcd.clear();
    lcd.print("player: ");
    lcd.print(firstPlayer);
    radio.stopListening();
    radio.openWritingPipe(addresses[0]);
    data[0] = RADIO_FIRST;
    data[1] = firstPlayer;
    for (int i = 0; i < 3; i++)
      radio.write(&data, sizeof(data));
    state = MASTER_ASSESS;
  }
  
  if (getButton() == BTN_RED)
  {
    Serial.println("go to IDLE");
    lcd.clear();
    lcd.print("Cancelling...");
    ledOff(LED_RED);
    state = MASTER_IDLE; 
  }
}


void mAssess()
{
  uint8_t data[2];
  uint8_t buttn;

  ledBlink(LED_BOTH);
  buttn = getButton();
  if (buttn != BTN_NONE)
  {
    radioEmptyRxQueues();
    while (radio.available())
      radio.read(data,sizeof(data)); //read remaining data in buffers to clean it up
    switch(buttn)
    {
      case BTN_RED:  
        state = MASTER_WRONG;
        break;
      case BTN_GREEN:
        state = MASTER_RIGHT;
        break;
      default:
        break;
    }
  }
}

void mRight()
{
  uint8_t data[3];

  score[firstPlayer]++;
  
  data[0] = RADIO_RIGHT;
  data[1] = firstPlayer;
  data[2] = score[firstPlayer];
  
  radio.openWritingPipe(addresses[0]);
  radio.write(data, sizeof(data));
  ledOff(LED_BOTH);
  state = MASTER_IDLE;
}

void mWrong()
{
  uint8_t data[2];
  
  data[0] = RADIO_WRONG;
  data[1] = firstPlayer;
  
  radio.openWritingPipe(addresses[0]);
  radio.write(&data, sizeof(data));
 
  ledOff(LED_BOTH);
  state = MASTER_IDLE;
}



void loop()
{
  buttonScan();

  switch(state)
  {
    case MASTER_IDLE:
      mIdle();
      break;
    case MASTER_WAIT:
      mWait();
      break;
    case MASTER_ASSESS:
      mAssess();
      break;
    case MASTER_RIGHT:
      mRight();
      break;
    case MASTER_WRONG:
      mWrong();
      break;
    default:
      break;
  }
}

  
