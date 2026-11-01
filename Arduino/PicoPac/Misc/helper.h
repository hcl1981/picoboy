#include "hardware/pll.h"
#include "hardware/clocks.h"

// Matthias Busse 5.2014
void eepromWriteInt(int adr, int wert)
{
  byte low, high;
  low = wert & 0xFF;
  high = (wert >> 8) & 0xFF;
  EEPROM.write(adr, low);
  EEPROM.write(adr + 1, high);
  EEPROM.commit();
  return;
}

// Matthias Busse 5.2014
int eepromReadInt(int adr)
{
  byte low, high;
  low = EEPROM.read(adr);
  high = EEPROM.read(adr + 1);
  return low + ((high << 8) & 0xFF00);

  return 0;
}