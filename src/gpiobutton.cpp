#include "gpiobutton.h"
#include "logengine.h"
#include <Arduino.h>

using namespace ModFirmWare;

#define LOGTAG "gpiobtn"
#define INVALID_PIN 255


GPIOButton::GPIOButton(uint8_t pin, uint8_t mode, bool inverted):
  Buttons(), pin(pin), inverted(inverted), mode(mode)
//****************************************************************************************
{
  

}

void GPIOButton::setupButtons()
//****************************************************************************************
{
  if ((INPUT == mode) || (INPUT_PULLUP == mode))
  {
    pinMode(pin, mode);
    logger->info(LOGTAG, "Setup pin %d as %s", pin, (INPUT == mode) ? "INPUT" : "INPUT_PULLUP");
  }
  else
  {
    logger->error(LOGTAG, "You assigned the wrong pin mode use either INPUT or INPUT_PULLUP. Pin %d will be ignored!", pin);
    pin = INVALID_PIN;
  }

}

uint16_t GPIOButton::readButtons()
//****************************************************************************************
{
  uint16_t value = digitalRead(pin);
  if (inverted)
  {
    value = (HIGH == value) ? LOW : HIGH;
  }

  return value;
}