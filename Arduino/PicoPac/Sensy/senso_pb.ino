#include <U8g2lib.h>
#include <SPI.h>
#include "tiles.h"

#define LEDR 5
#define LEDY 6
#define LEDG 7
#define KEY_RIGHT 2
#define KEY_DOWN 3
#define KEY_LEFT 4
#define KEY_UP 1
#define KEY_CENTER 0
#define SPEAKER 15

const int buttonPins[] = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}; // Pins, an denen die Taster angeschlossen sind
int buttonStates[] = {HIGH, HIGH, HIGH, HIGH}; // Aktuelle Zustände der Taster
int lastButtonStates[] = {HIGH, HIGH, HIGH, HIGH}; // Vorherige Zustände der Taster
unsigned long lastDebounceTimes[] = {0, 0, 0, 0}; // Zeitstempel der letzten Änderungen

// Entprellzeit in Millisekunden
unsigned long debounceDelay = 5;


U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, 10, 8, 9);

#define A_SIZE 19

byte history [10000];
int progress = 0;

bool rungame = true;
bool repaint = false;
unsigned long int  repainttime;
unsigned long int starttime;
unsigned long int lastmillis = millis();


void drawCircles(bool filled) {
  if (!filled) {
    u8g2.drawCircle(64, 32, 8);
  } else {
    u8g2.drawDisc(64, 32, 8);
  }
  u8g2.drawCircle(64, 32, 31);

  //drawUp(64 - 9, 32 - 9 - 20);
  //drawDown(64 - 9, 32 + 19 - 9 + 19);
  //drawLeft(64 - 9 - 20, 32 - 9);
  //drawRight(64 + 19 - 9 + 19, 32 - 9);

}

void drawLeft(int x, int y) {
  for (int i = 0; i < A_SIZE; i++) {
    for (int j = 0; j < A_SIZE; j++) {
      if (arrow[i][j] == 1) {
        u8g2.drawPixel(x + i, y + j);
      }
    }
  }
}

void drawRight(int x, int y) {
  for (int i = 0; i < A_SIZE; i++) {
    for (int j = 0; j < A_SIZE; j++) {
      if (arrow[i][j] == 1) {
        u8g2.drawPixel(x - i, y + j);
      }
    }
  }
}

void drawUp(int x, int y) {
  for (int i = 0; i < A_SIZE; i++) {
    for (int j = 0; j < A_SIZE; j++) {
      if (arrow[i][j] == 1) {
        u8g2.drawPixel(x + j, y + i);
      }
    }
  }
}

void drawDown(int x, int y) {
  for (int i = 0; i < A_SIZE; i++) {
    for (int j = 0; j < A_SIZE; j++) {
      if (arrow[i][j] == 1) {
        u8g2.drawPixel(x + j, y - i);
      }
    }
  }
}

int repeatAdd() {
  int dir = random(4);
  history[progress] = dir;
  for (int i = 0; i <= progress; i++) {
    u8g2.clearBuffer();
    //u8g2.setCursor(120, 59);
    //u8g2.print("WATCH");
    u8g2.setCursor(10,15);
    u8g2.println("W");
    u8g2.setCursor(10,26);
    u8g2.println("A");
    u8g2.setCursor(10,37);
    u8g2.println("T");
    u8g2.setCursor(10,48);
    u8g2.println("C");
    u8g2.setCursor(10,59);
    u8g2.println("H");
    drawCircles(false);

    u8g2.sendBuffer();
    delay(300);
    if (history[i] == 0) {
      drawUp(64 - 9, 32 - 9 - 20);//-19
    }
    if (history[i] == 1) {
      drawDown(64 - 9, 32 + 19 - 9 + 19);
    }
    if (history[i] == 2) {
      drawLeft(64 - 9 - 20, 32 - 9);
    }
    if (history[i] == 3) {
      drawRight(64 + 19 - 9 + 19, 32 - 9);
    }
    //u8g2.setCursor(120, 59);
    //u8g2.print("WATCH");
    u8g2.setCursor(10,15);
    u8g2.println("W");
    u8g2.setCursor(10,26);
    u8g2.println("A");
    u8g2.setCursor(10,37);
    u8g2.println("T");
    u8g2.setCursor(10,48);
    u8g2.println("C");
    u8g2.setCursor(10,59);
    u8g2.println("H");
    drawCircles(false);
    u8g2.sendBuffer();
    delay(500);
  }

  u8g2.clearBuffer();
  drawCircles(false);

  //u8g2.setCursor(120, 59);
  //u8g2.print("WATCH");
    u8g2.setCursor(10,15);
    u8g2.println("W");
    u8g2.setCursor(10,26);
    u8g2.println("A");
    u8g2.setCursor(10,37);
    u8g2.println("T");
    u8g2.setCursor(10,48);
    u8g2.println("C");
    u8g2.setCursor(10,59);
    u8g2.println("H");
  u8g2.sendBuffer();
  delay(500);
  u8g2.clearBuffer();
  drawCircles(false);

  u8g2.sendBuffer();
  progress++;
  return dir;
}

void play() {
  while (rungame) {

    int current = repeatAdd();



    int richtung = - 1;


    //richtung abfragen anfang
    for (int j = 0; j < progress && rungame;) {
      for (int i = 0; i < 4 && rungame; i++) {
        int reading = digitalRead(buttonPins[i]);


        u8g2.clearBuffer();
        if (digitalRead(KEY_UP) == LOW) drawUp(64 - 9, 32 - 9 - 20);
        if (digitalRead(KEY_DOWN) == LOW)drawDown(64 - 9, 32 + 19 - 9 + 19);
        if (digitalRead(KEY_LEFT) == LOW)drawLeft(64 - 9 - 20, 32 - 9);
        if (digitalRead(KEY_RIGHT) == LOW)drawRight(64 + 19 - 9 + 19, 32 - 9);
        u8g2.setCursor(120, 63);
        u8g2.println("REPEAT");
        drawCircles(true);

        u8g2.sendBuffer();

        if (reading != lastButtonStates[i]) {
          lastDebounceTimes[i] = millis();
        }
        if ((millis() - lastDebounceTimes[i]) > debounceDelay) {
          if (reading != buttonStates[i]) {
            buttonStates[i] = reading;
            if (buttonStates[i] == LOW) {
              if (history[j] == i) {
                j++;
              } else {
                rungame = false;
              }
            }
          }
        }
        lastButtonStates[i] = reading;
      }
    }
    while (digitalRead(KEY_UP) == LOW || digitalRead(KEY_DOWN) == LOW || digitalRead(KEY_LEFT) == LOW || digitalRead(KEY_RIGHT) == LOW ) {}
    u8g2.clearBuffer();
    u8g2.setCursor(120, 63);
    u8g2.print("REPEAT");
    drawCircles(true);

    if (rungame) {
      digitalWrite(LEDG, HIGH);
      delay(50);
      digitalWrite(LEDG, LOW);
    } else {
      digitalWrite(LEDR, HIGH);
      delay(50);
      digitalWrite(LEDR, LOW);

    }
    u8g2.sendBuffer();

    delay(300);


  }
}

void intro() {
  //u8g2.setFont(u8g2_font_michaelmouse_tu);
  while (digitalRead(KEY_CENTER) == HIGH) {
    u8g2.clearBuffer();
    u8g2.setCursor(5, 23);
    u8g2.println("watch");
    u8g2.println("&");
    u8g2.println("repeat");
    u8g2.drawRFrame(0, 5, 128, 53, 8);

    if (millis() % 400 > 200) {
      u8g2.setFontDirection(0);
      u8g2.setCursor(35, 50);
      u8g2.print("PRESS!");
    }
    u8g2.sendBuffer();
  }
  u8g2.clearBuffer();
  delay(10);
  while (digitalRead(KEY_CENTER) == LOW) {}
}

void setup(void)
{
  Serial.begin(9600);

  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();

  pinMode(KEY_RIGHT, INPUT_PULLUP);
  pinMode(KEY_DOWN, INPUT_PULLUP);
  pinMode(KEY_LEFT, INPUT_PULLUP);
  pinMode(KEY_UP, INPUT_PULLUP);
  pinMode(KEY_CENTER, INPUT_PULLUP);

  pinMode(SPEAKER, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDY, OUTPUT);
  u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);


  while (true) {
    progress = 0;

    rungame = true;
    repaint = false;

    intro();
    randomSeed(millis());
    //u8g2.setFontDirection(3);


    play();


    u8g2.clearBuffer();
    u8g2.setCursor(120, 63);
    u8g2.print("SCORE");
    u8g2.print(progress - 1);
    drawCircles(false);

    u8g2.sendBuffer();
    delay(500);
    while (digitalRead(KEY_CENTER) == HIGH) {}
    u8g2.clearBuffer();
    //drawCircles();

    u8g2.sendBuffer();


    delay(100);
    while (digitalRead(KEY_CENTER) == LOW) {}

  }
}


void loop() {
}
