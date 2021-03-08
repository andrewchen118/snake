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

int GAME_SPEED = NORMAL;
char TRAJECTORY = RIGHT;
int PREVIOUS[2] = {0,0};
int FOOD[2] = {-1,-1};
int SNAKE[100][2];
int HEAD = 0;
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
void setGameSpeed(int game_speed);

unsigned long time = millis();

void setup(){
  Serial.begin(9600);
  
  pinMode(Jx, INPUT);
  pinMode(Jy, INPUT);
  pinMode(SW, INPUT_PULLUP); 
  
  tft.begin();                     
  tft.setRotation(3);           

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextWrap(false);
  tft.setTextSize(2); 

  tft.setTextColor(ILI9341_RED);            
  tft.setCursor(85,5);              
  tft.print("Porofound pog l o l"); 
  
  tft.setTextColor(ILI9341_GREEN);  
  tft.setCursor(20,220);           
  tft.print("Porofound pog...");

  elongate();

  setGameSpeed(NORMAL);
}
 
void loop()
{
  setTrajectory();
  if ( getSW() == 0 ) {
    cookFood();
  }
  if ( millis() - time >= GAME_SPEED ) {
    
    if (elongate()) {
      cookFood();
    } else {
      clearTail();
    }
    moveNext();
    
    time = millis();
  }

  Serial.print("X: ");
  Serial.print(getJx());
  Serial.print(" | Y: ");
  Serial.print(getJy());
  Serial.print(" | Button: ");
  Serial.print(getSW());
  Serial.print(" | Food: ");
  Serial.print(FOOD[0]);
  Serial.print(", ");
  Serial.println(FOOD[1]);
}

void moveNext() {
  updateBody();
  setHead();

  if ( getPosX() >= width || getPosX() <= 0 ) {
    tft.setCursor(bSize*8,bSize*8);
  } else if ( getPosY() >= height || getPosY() <= 0 ) {
    tft.setCursor(bSize*8,bSize*8);
  } else {
    tft.fillRect(SNAKE[HEAD][X], SNAKE[HEAD][Y], bSize, bSize, ILI9341_GREEN);

  }
}

bool elongate() {
  if ( getPosX() == FOOD[0] && getPosY() == FOOD[1] ) {
    HEAD++;
    // updateBody();
    setHead();

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
    // SNAKE[HEAD][X] = getPosX();
    SNAKE[HEAD][Y] = getPosY() - bSize;
  } else if ( TRAJECTORY == DOWN ) {
    // SNAKE[HEAD][X] = getPosX();
    SNAKE[HEAD][Y] = getPosY() + bSize;
  } else if ( TRAJECTORY == RIGHT ) {
    SNAKE[HEAD][X] = getPosX() + bSize;
    // SNAKE[HEAD][Y] = getPosY();
  } else if ( TRAJECTORY == LEFT ) {
    SNAKE[HEAD][X] = getPosX() - bSize;
    // SNAKE[HEAD][Y] = getPosY();
  }
  tft.setCursor(SNAKE[HEAD][X], SNAKE[HEAD][Y]);
}

void clearTail() {
  // tft.fillRect(PREVIOUS[0], PREVIOUS[1], bSize, bSize, ILI9341_BLACK);
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
  FOOD[0] = random(0,width/bSize)*bSize;
  FOOD[1] = random(0,height/bSize)*bSize;
  tft.fillRect(FOOD[0], FOOD[1], bSize, bSize, ILI9341_WHITE);
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
