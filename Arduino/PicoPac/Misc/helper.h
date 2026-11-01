#include "hardware/pll.h"
#include "hardware/clocks.h"

// Legt einen 16-Bit-Wert in zwei Bytes ab, das niedrige zuerst
void eepromWriteInt(int adr, int wert)
{
  for (int i = 0; i < 2; i++)
    EEPROM.write(adr + i, (wert >> (8 * i)) & 0xFF);
  EEPROM.commit();
}

// Gegenstueck zu eepromWriteInt
int eepromReadInt(int adr)
{
  return EEPROM.read(adr) | (EEPROM.read(adr + 1) << 8);
}