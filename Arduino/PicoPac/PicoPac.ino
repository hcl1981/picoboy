#include <EEPROM.h>
#include <U8g2lib.h>
#include <SPI.h>

#define LEDR 5
#define LEDY 6
#define LEDG 7
#define KEY_RIGHT 2
#define KEY_DOWN 3
#define KEY_LEFT 4
#define KEY_UP 1
#define KEY_CENTER 0
#define SPEAKER 15

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R3, /* cs=*/13, /* dc=*/11, /* reset=*/12);
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R3, /* cs=*/10, /* dc=*/8, /* reset=*/9);

#include "Misc/helper.h"

#include "TABman/tabman.h"
#include "SCLange/sclange.h"
#include "PICtris/pictris.h"
#include "Misc/StarIntro.h"
#include "Demo/demo.h"
#include "Conway/conway.h"
#include "HWTest/hwtest.h"

void setup(void)
{
  set_sys_clock_khz(50000, true);
  Serial.begin(9600);

  EEPROM.begin(256);

  if (eepromReadInt(0) >= 65500)
  {
    eepromWriteInt(0, 0);
  }
  if (eepromReadInt(2) >= 65500)
  {
    eepromWriteInt(2, 0);
  }
  if (eepromReadInt(4) >= 65500)
  {
    eepromWriteInt(4, 0);
  }
  
  u8g2.begin();

  u8g2.setContrast(31);
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

  StarIntro intro(u8g2);
  int gameID = intro.starIntro();

  randomSeed(millis());
  if (gameID == 1)
  {
    PICtris pic(u8g2);
    pic.run();
  }
  else if (gameID == 2)
  {
    SCLange scl(u8g2);
    scl.run();
  }
  else if (gameID == 3)
  {
    TABman tab(u8g2);
    tab.run();
  }
  else if (gameID == 4)
  {
    Demo dem(u8g2);
    dem.run();

  }
  else if (gameID == 5)
  {
    Conway con(u8g2);
    con.run();

  }
  else if (gameID == 6)
  {
    HWTest hwt(u8g2);
    hwt.run();
  }
}

void loop() {}
