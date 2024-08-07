#include "roteryencoder.h"
#include "modfirmware.h"
#include <limits.h>

using namespace ModFirmWare;

#ifndef ROTARY_ENCODER_DEBOUNCE
#define ROTARY_ENCODER_DEBOUNCE 0
#endif

#define LOGTAG "rotary"

/****
  States of rotary encoder transitions
  0000: no movement
  0001: counter-clockwise
  0010: clockwise
  0011: invalid state
  0100: clockwise
  0101: no movement
  0110: invalid state
  0111: counter-clockwise
  1000: counter-clockwise
  1001: invalid state
  1010: no movement
  1011: clockwise
  1100: invalid state
  1101: clockwise
  1110: counter-clockwise
  1111: no movement
*/
#define ROTARY_ENCODER_TRANSITIONS { \
    0, -1, 1, 0,                     \
    1, 0, 0, -1,                     \
    -1, 0, 0, 1,                     \
    0, 1, -1, 0}

const int8_t transitions[16] = ROTARY_ENCODER_TRANSITIONS;

RotaryEncoder::RotaryEncoder(int clockPin, int dataPin): Component()
//****************************************************************************************
{
  initialize(clockPin, dataPin);
}

RotaryEncoder::RotaryEncoder(int clockPin, int dataPin, long min, long max) : Component()
{
  initialize(clockPin, dataPin);
  setMaximum(max);
  setMinimum(min);
}
RotaryEncoder::RotaryEncoder(int clockPin, int dataPin, long min, long max, bool rotate)
: Component()
{
  initialize(clockPin, dataPin);
  setMaximum(max);
  setMinimum(min);
  setRotate(rotate);
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
  lastState = (digitalRead(clockPin) << 1) | digitalRead(dataPin);

  attachInterruptArg(digitalPinToInterrupt(clockPin), registerTick, this, CHANGE);
  attachInterruptArg(digitalPinToInterrupt(dataPin), registerTick, this, CHANGE);

  logger->debug(LOGTAG, "Minimum = %d, Maximum = %d, Rotate = %s", minimum, maximum, rotate ? "on": "off");
  return true;
}

void RotaryEncoder::loop()
//****************************************************************************************
{
  
  // Temporarily disable interrupts to safely access shared variables
  noInterrupts();
  long ticks = counter - lastCounter;
  lastCounter = counter;
  interrupts();

  rotaryCounter = capAndRotate(rotaryCounter + ticks);

  if (ticks == 0)
  {
    return;
  }
 
  logger->debug(LOGTAG, "Counter = %d RotaryCounter = %d", counter, rotaryCounter);

  while (ticks > 0)
  {
    if (onCw)
    {
      onCw(rotaryCounter);
    }
    --ticks;
  }

  while (ticks < 0)
  {
    if (onCcw)
    {
      onCcw(rotaryCounter);
    }
    ++ticks;
  }
 
}

long RotaryEncoder::capAndRotate(long counter)
//****************************************************************************************
{
  if ((maximum >= counter) && (minimum <= counter))
  {
    return counter;
  }

  counter = (maximum < counter) ? maximum : counter;
  counter = (minimum > counter) ? minimum : counter;

  if (rotate)
  {
    counter = (maximum == counter) ? minimum : maximum;
  }

  return counter;
}

void RotaryEncoder::initialize(int cp, int dp)
//****************************************************************************************
{
  clockPin = cp;
  dataPin = dp;
  counter = 0;
  rotaryCounter = 0;
  lastTick = 0; 
  lastCounter = 0;
  movement = 0;
  minimum = LONG_MIN;
  maximum = LONG_MAX;
  rotate = false;
  onCcw = NULL;
  onCw = NULL;
}

void IRAM_ATTR RotaryEncoder::registerTick(void *arg)
//****************************************************************************************
{
  RotaryEncoder *encoder = static_cast<RotaryEncoder *>(arg);
  long now = millis();

  if (ROTARY_ENCODER_DEBOUNCE > (now - encoder->lastTick))
  {
    return;
  }
  encoder->lastTick = millis();

  encoder->state = (digitalRead(encoder->clockPin) << 1) | digitalRead(encoder->dataPin);
  if (encoder->state == encoder->lastState)
  {
    return;
  }

  encoder->movement += transitions[(encoder->lastState << 2) | encoder->state];

  if (3 == encoder->state)
  {
    encoder->counter = encoder->movement >> 2;    
  }

  encoder->lastState = encoder->state;
}