#include "roteryencoder.h"
#include "modfirmware.h"

using namespace ModFirmWare;

RotaryEncoder::RotaryEncoder(int clockPin, int dataPin)
    : clockPin(clockPin), dataPin(dataPin), counter(0),
      onCcw(nullptr), onCw(nullptr)
//****************************************************************************************
{
}

bool RotaryEncoder::setup(Application *app)
//****************************************************************************************
{
  if (!Component::setup(app))
  {
    return false;
  }

  pinMode(clockPin, INPUT);
  pinMode(dataPin, INPUT);
  attachInterruptArg(digitalPinToInterrupt(clockPin), registerTick, this, CHANGE);
  return true;
}

void RotaryEncoder::loop()
//****************************************************************************************
{
  // Temporarily disable interrupts to safely access shared variables
  noInterrupts();
  int8_t ticks = counter;
  counter = 0;
  interrupts();

  if (0 == ticks) 
  {
    return;
  }

  while (ticks > 0)
  {
    if (onCw)
    {
      onCw();
    }
    --ticks;
  }

  while (ticks < 0)
  {
    if (onCcw)
    {
      onCcw();
    }
    ++ticks;
  }
}

void IRAM_ATTR RotaryEncoder::registerTick(void *arg)
//****************************************************************************************
{
  RotaryEncoder *encoder = static_cast<RotaryEncoder *>(arg);
  if (digitalRead(encoder->clockPin) == digitalRead(encoder->dataPin))
  {
    encoder->counter++;
  }
  else
  {
    encoder->counter--;
  }
}