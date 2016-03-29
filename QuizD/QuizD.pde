import ddf.minim.*;
Minim minim;
AudioPlayer player;

//===================================================================================
// Test questions
//===================================================================================
XML xml;
XML[] tests; 
int currentTest = 0;
int[] score;
int maxPlayers = 6;
 
public enum State {
  STATE_INIT,
  STATE_CONFIG,
  STATE_IDLE,
  STATE_WAIT,
  STATE_TIMEOUT, 
  STATE_ASSESS,
  STATE_RIGHT,
  STATE_WRONG,
};

State state;

PImage bg;
int x=0;
int y= 0;

int bgSizeX = 1800;
int bgSizeY = 850;

void setup() {
  size(1800, 850, P3D); 
  minim = new Minim(this);
  textAlign(CENTER);
  colorMode(RGB, 100);
  strokeWeight(4);
  fill(90, 100, 0);
  stroke(100,100,80);  
  textSize(70);
  bg = loadImage("space.png");
  background(bg);
  text("Press any key to start", bgSizeX/2, bgSizeY/2);
  xml = loadXML("quiz.xml");
  tests = xml.getChildren("test");
  score = new int[maxPlayers];
  for (int i = 0; i < maxPlayers; i++) {
    score[i] = 0;
  } 
  state = State.STATE_INIT;
}

void draw() { 
 display();  
}

static int textSize;
//===================================================================================
// States
//===================================================================================

//  INIT -> IDLE -> WAIT -> ASSESS -> RIGHT 
//            ^      |     /   |        |
//            |      v    /    v        |
//            +--- TIMEOUT   WRONG      | 
//            |                |        | 
//            +----------------+--------+ 


//===================================================================================
// INIT state
//===================================================================================
void qInit() {
  if (getKey()!=0) {
    state = State.STATE_IDLE;
    textSize = 50;
  }
}
 
//===================================================================================
// IDLE state
//===================================================================================
long timeTick= 0;
boolean dispScore = false;
void qIdle() {
  if (millis() > timeTick + 3000) {
    timeTick = millis(); 
    textSize(70);
    background(bg);
    if (dispScore == true) {
      displayScore();
      dispScore = false;       
    }
    else {
      text("GET READY!", bgSizeX/2, bgSizeY/2);
      dispScore = true; 
  }
}
  if (getKey()!=0) {
       qIdle2Wait();
  }
}

int timeLimit = 10;
int timeoutBarX = 50;
int timeoutRed = 0;
int timeoutGreen = 100;
int timeout;

void qIdle2Wait() {
  PImage img;
  String imageStr = tests[currentTest].getString("image");
  String question = tests[currentTest].getContent();
  background(bg);
  displayQuestionNumber();
  if (imageStr != null) {
    img=loadImage(imageStr);
    imageMode(CENTER);
    image(img, bgSizeX/2, bgSizeY/2-200);
  }
  
  textAlign(CENTER);
  fill(100, 100, 0);
  text(question, bgSizeX/2, bgSizeY/2, timeout*2);    
  timeout = timeLimit;
  timeTick = millis();
  timeoutBarX = 20;
  state = State.STATE_WAIT;
}

//===================================================================================
// WAIT state
//===================================================================================
void qWait() {
  if (millis() > timeTick + 1000) {
    timeTick = millis();  
    timeout--;
    player = minim.loadFile("tick.wav"); 
    player.play();
  }
  if (timeout > 2*timeLimit/3) {
    timeoutRed = 0;
    timeoutGreen = 100;    
  }
  else if ((timeout > timeLimit/3) && (timeout <= 2*timeLimit/3)) {
    timeoutRed++;
  }
  else if (timeout < timeLimit/3) {
    timeoutRed = timeoutRed++;
    timeoutGreen--;
  }
  
  stroke(timeoutRed, timeoutGreen, 0);
  line (timeoutBarX, bgSizeY - 60, timeoutBarX, bgSizeY - 10);
  timeoutBarX = timeoutBarX+3;

  if (timeout == 0) {
      qWait2Timeout();
    }
  
  if (getKey() != 0) {
    qWait2Assess();  
  }
}

void qWait2Timeout() { 
    textAlign(CENTER);
    background(bg);
    displayQuestionNumber();
    String question = tests[currentTest].getString("answer");
    fill(0, 255, 0);
    text("Correct answer:", bgSizeX/2, bgSizeY/2-100);
    text(question, bgSizeX/2, bgSizeY/2);
    player = minim.loadFile("time-s-up.wav"); 
    player.play();
    state = State.STATE_TIMEOUT;
}

void qWait2Assess() {
  textAlign(CENTER);
  background(bg);
  displayQuestionNumber();
  fill(0, 255, 0);
  text("Player:"+key, bgSizeX/2, bgSizeY/2-100);
  text("Waiting for the answer"+key, bgSizeX/2, bgSizeY/2);
    
  state = State.STATE_ASSESS;
}

//===================================================================================
// TIMEOUT state
//===================================================================================
void qTimeout() {
  if (millis() > timeTick + 10000) {
    timeTick = millis();
    currentTest++;
    if (currentTest == tests.length)
      currentTest = 0;
    state = State.STATE_IDLE;
  }
}

//===================================================================================
// ASSESS state
//===================================================================================
void qAssess() {
  int key;
  
  textAlign(CENTER);
  background(bg);
  displayQuestionNumber();
  text("Waiting for the answer...", bgSizeX/2, bgSizeY/2);
    
  key = getKey();
  if (key =='r') {
    qAssess2Right();
  } else if (key =='w') {
    qAssess2Wrong();
  }
}

void qAssess2Right() {
  textAlign(CENTER);
  background(bg);
  displayQuestionNumber();
  String question = tests[currentTest].getString("answer");
  fill(0, 255, 0);
  text("That's correct:", bgSizeX/2, bgSizeY/2-100);
  text(question, bgSizeX/2, bgSizeY/2);
  player = minim.loadFile("fanfare.wav"); 
  player.play();

  state = State.STATE_RIGHT;
}

void qAssess2Wrong() {
  textAlign(CENTER);
  background(bg);
  displayQuestionNumber();
  String question = tests[currentTest].getString("answer");
  fill(255, 0,0);
  text("That's incorrect. Right answer:", bgSizeX/2, bgSizeY/2-100);
  fill(255, 255, 0);
  text(question, bgSizeX/2, bgSizeY/2);
  player = minim.loadFile("wrong.wav"); 
  player.play();

  state = State.STATE_WRONG;
}

//===================================================================================
// RIGHT state
//===================================================================================
void qRight() {
  if (millis() > timeTick + 10000) {
    timeTick = millis();
    currentTest++;
    if (currentTest == tests.length)
      currentTest = 0;
    state = State.STATE_IDLE;
  }
}
//===================================================================================
// WRONG state
//===================================================================================
void qWrong() {
  if (millis() > timeTick + 10000) {
    timeTick = millis();
    currentTest++;
    if (currentTest == tests.length)
      currentTest = 0;
    state = State.STATE_IDLE;
  }
}

void display() {
  switch (state) {
    case STATE_INIT:
      qInit();
      break;
    case STATE_CONFIG:
    break;
    case STATE_IDLE:
      qIdle();
      break;
    case STATE_WAIT:
      qWait();
      break;
    case STATE_TIMEOUT:
      qTimeout();
      break;
    case STATE_ASSESS:
      qAssess();
      break; 
    case STATE_RIGHT:
      qRight();
      break;
    case STATE_WRONG:
      qWrong();
      break;
  }
}

//===================================================================================
// Helper functions
//===================================================================================
void displayQuestionNumber() {
  fill(255, 255, 155);
  textAlign(LEFT);
  textSize(30);
  text("Question: "+ (currentTest+1) + "/" + tests.length , 60, 60);  
  textAlign(CENTER);
  textSize(70); // return to default size
}

void displayScore() {
 text("Scores:",bgSizeX/2, 120);  
  for (int i = 0; i < maxPlayers; i++) {
    text("Player " + (i+1) + ": "+score[i], 400, 200 + 80*i);
  } 
}

int keyIndex = 0;
int getKey()
{
    int key = keyIndex;   
    keyIndex = 0; 
    //println("key:" + key);
    return key;
}

void keyPressed() {
  keyIndex = key;
}