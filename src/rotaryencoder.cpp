#include "roteryencoder.h"
#include "modfirmware.h"

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

RotaryEncoder::RotaryEncoder(int clockPin, int dataPin)
    : clockPin(clockPin), dataPin(dataPin), counter(0), lastTick(0), lastCounter(0), movement(0),
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
  lastState = (digitalRead(clockPin) << 1) | digitalRead(dataPin);

  attachInterruptArg(digitalPinToInterrupt(clockPin), registerTick, this, CHANGE);
  attachInterruptArg(digitalPinToInterrupt(dataPin), registerTick, this, CHANGE);
  return true;
}

void RotaryEncoder::loop()
//****************************************************************************************
{
  
  // Temporarily disable interrupts to safely access shared variables
  noInterrupts();
  long ticks = counter - lastCounter;
  long c = counter;
  lastCounter = counter;
  interrupts();

  if (ticks == 0)
  {
    return;
  }
 
  logger->debug(LOGTAG, "Counter = %d ", c);

  while (ticks > 0)
  {
    if (onCw)
    {
      onCw(c);
    }
    --ticks;
  }

  while (ticks < 0)
  {
    if (onCcw)
    {
      onCcw(c);
    }
    ++ticks;
  }
 
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