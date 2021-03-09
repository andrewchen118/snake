#include "Adafruit_GFX.h"     
#include "Adafruit_ILI9341.h"

// TFT PINS
#define TFT_DC 9              
#define TFT_CS 10
#define TFT_RST 8
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_CLK 13

// JOYSTICK
#define Jx A1
#define Jy A3
#define SW 4

// GAME CONSTANTS
#define bSize 12
#define SLOW 500
#define NORMAL 250
#define FAST 100
#define UP 'U'
#define DOWN 'D'
#define LEFT 'L'
#define RIGHT 'R'
#define width 320
#define height 240
#define X 0
#define Y 1

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

unsigned long time;
bool GAME_START;
int GAME_SPEED;
char TRAJECTORY;
int FOOD[2];
int SNAKE[100][2];
int HEAD = 1;
int TAIL = 0;

int getJx();
int getJy();
int getSW();

int getPosX();
int getPosY();

void moveNext();
void cookFood();
bool elongate();
void setHead();
void updateBody();
void clearTail();
void setTrajectory(char dir);
bool didCollide();

void startGame();
void gameOver();
void setGameSpeed(int game_speed);

void setup(){
  Serial.begin(9600);
  
  pinMode(Jx, INPUT);
  pinMode(Jy, INPUT);
  pinMode(SW, INPUT_PULLUP);

  randomSeed(millis());
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);

  gameStart();
}
 
void loop()
{
  setTrajectory();
  if ( millis() - time >= GAME_SPEED && GAME_START ) {
    moveNext();
    time = millis();
  }
}

void gameStart() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(4);
  tft.setCursor(95,90);
  tft.print("Snake!");
  tft.setCursor(95,122);
  tft.setTextSize(2);
  tft.print("by A1");
  tft.setCursor(80,186);
  tft.setTextSize(1);
  tft.print("press the joystick to start");

  while ( !GAME_START ) {
    if ( getSW() == 0 ) {
      tft.setTextColor(ILI9341_BLACK);
      tft.setTextSize(4);
      tft.setCursor(95,90);
      tft.print("Snake!");
      tft.setCursor(95,122);
      tft.setTextSize(2);
      tft.print("by A1");
      tft.setCursor(80,186);
      tft.setTextSize(1);
      tft.print("press the joystick to start");

      GAME_START = true;
      TRAJECTORY = RIGHT;
      setGameSpeed(NORMAL);
      SNAKE[HEAD][X] = (width/bSize/2)*bSize;
      SNAKE[HEAD][Y] = (height/bSize/2)*bSize;
      tft.setCursor(SNAKE[HEAD][X], SNAKE[HEAD][Y]);
      cookFood();
      time = millis();
    }
  }
}

void gameOver() {
  GAME_START = false;
  tft.fillRect(FOOD[X], FOOD[Y], bSize, bSize, ILI9341_BLACK);

  // draw game over
  tft.setTextSize(4);
  tft.setTextColor(0x6000);
  tft.setCursor(52,93);
  tft.print("GAME OVER");
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(55,90);
  tft.print("GAME OVER");
  delay(3000);

  // remove everythin on screen and reset values
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(52,93);
  tft.print("GAME OVER");
  tft.setCursor(55,90);
  tft.print("GAME OVER");
  for ( int i = 0; i < HEAD + 1; i++) {
    tft.fillRect(SNAKE[i][X], SNAKE[i][Y], bSize, bSize, ILI9341_BLACK);
    SNAKE[i][X] = 0;
    SNAKE[i][Y] = 0;
  }
  HEAD = 1;

  gameStart();
}

void moveNext() {
  if (elongate()) {
    cookFood();
  } else {
    clearTail();
    updateBody();
  }
  setHead();

  if (!didCollide()) {
    tft.fillRect(SNAKE[HEAD][X], SNAKE[HEAD][Y], bSize, bSize, ILI9341_GREEN);
  } else {
    gameOver();
  }
}

bool elongate() {
  if ( getPosX() == FOOD[0] && getPosY() == FOOD[1] ) {
    HEAD++;
    SNAKE[HEAD][X] = getPosX();
    SNAKE[HEAD][Y] = getPosY();
    return true;
  }
  return false;
}

void updateBody() {
  for (int i = 0; i < HEAD; i++) {
    SNAKE[i][X] = SNAKE[i+1][X];
    SNAKE[i][Y] = SNAKE[i+1][Y];
  }
}

void setHead() {
  if (TRAJECTORY == UP) {
    SNAKE[HEAD][Y] = getPosY() - bSize;
  } else if ( TRAJECTORY == DOWN ) {
    SNAKE[HEAD][Y] = getPosY() + bSize;
  } else if ( TRAJECTORY == RIGHT ) {
    SNAKE[HEAD][X] = getPosX() + bSize;
  } else if ( TRAJECTORY == LEFT ) {
    SNAKE[HEAD][X] = getPosX() - bSize;
  }
  tft.setCursor(SNAKE[HEAD][X], SNAKE[HEAD][Y]);
}

void clearTail() {
  tft.fillRect(SNAKE[0][0], SNAKE[0][1], bSize, bSize, ILI9341_BLACK);
}

void setTrajectory() {
  int x = getJx();
  int y = getJy();
  if ( y > 200 && TRAJECTORY != DOWN ) {
    TRAJECTORY = UP;
  } else if ( y < -200 && TRAJECTORY != UP ) {
    TRAJECTORY = DOWN;
  } else if ( x > 200 && TRAJECTORY != LEFT ) {
    TRAJECTORY = RIGHT;
  } else if ( x < -200 && TRAJECTORY != RIGHT ) {
    TRAJECTORY = LEFT;
  }
}

void setGameSpeed(int game_speed) {
  GAME_SPEED = game_speed;
}

void cookFood() {
  bool willnotwork;

  do {
    FOOD[X] = random(0,width/bSize)*bSize;
    FOOD[Y] = random(0,height/bSize)*bSize;
    willnotwork = false;
    for (int i = 0; i < HEAD + 1; i++) {
      if (FOOD[X] == SNAKE[i][X] || FOOD[Y] == SNAKE[i][Y]) {
        willnotwork = true;
      }
    }
  }
  while (willnotwork);
  tft.fillRect(FOOD[X], FOOD[Y], bSize, bSize, ILI9341_WHITE);
}

bool didCollide() {
  if ( ( SNAKE[HEAD][X] >= width || SNAKE[HEAD][X] <= 0 ) || ( SNAKE[HEAD][Y] >= height || SNAKE[HEAD][Y] <= 0 ) ) {
    return true;
  }
  for (int i = 0; i < HEAD; i++) {
    if ( SNAKE[HEAD][X] == SNAKE[i][X] && SNAKE[HEAD][Y] == SNAKE[i][Y] ) {
      return true;
    }
  }
  return false;
}

int getPosX() {
  return tft.getCursorX();
}

int getPosY() {
  return tft.getCursorY();
}

int getJx() { 
  return map(analogRead(Jx), 0, 1023, -512, 512);
}

int getJy() {
  return map(analogRead(Jy), 0, 1023, -512, 512);
}

int getSW() {
  return digitalRead(SW);
}
