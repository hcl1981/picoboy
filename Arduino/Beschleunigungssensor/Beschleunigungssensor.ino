#include <U8g2lib.h>
#include <SPI.h>
#include "STK8BA58.h"

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, 10, 8, 9);
STK8BA58 sensor;

void setup(void)
{
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();

  sensor.init();
  u8g2.setFont(u8g2_font_crox4hb_tf);
}

void loop() {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 20);
  u8g2.print(sensor.xAcc());
  u8g2.print(" m/s^2");
  u8g2.setCursor(0, 40);
  u8g2.print(sensor.yAcc());
  u8g2.print(" m/s^2");
  u8g2.setCursor(0, 60);
  u8g2.print(sensor.zAcc());
  u8g2.print(" m/s^2");
  u8g2.sendBuffer();
  delay(100);
}
