#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H
#include <Arduino.h>
#include "modfw_component.h"

namespace ModFirmWare
{
  class Application;

  class RotaryEncoder : public Component
  {
  public:
    using Callback = std::function<void(long counter)>;
  
    RotaryEncoder(int clockPin, int dataPin);
    RotaryEncoder(int clockPin, int dataPin, long min, long max);
    RotaryEncoder(int clockPin, int dataPin, long min, long max, bool rotate);
    bool setup(Application*) override;
    void loop() override;
    void setOnClockwise(Callback cb) { onCw = cb; }
    void setOnCounterClockwise(Callback cb) { onCcw = cb; }

    void setMinimum(long min) { minimum = min; }
    void setMaximum(long max) { maximum = max; }
    void setRotate(bool r) { rotate = r; }

  protected:

    long maximum;
    long minimum;
    bool rotate;

    long rotaryCounter;

    long capAndRotate(long counter);
    void initialize(int cp, int dp);

  private:
    int clockPin;
    int dataPin;
    
    volatile uint8_t state; // To store the current state of the encoder  
    volatile uint8_t lastState; // To store the last state of the encoder    
    volatile long counter; // Counter for encoder ticks (signed for both directions)
    volatile long movement; //the internal counter of the interrupt routine
    volatile long lastTick;
    volatile long lastCounter;
    
    Callback onCw;
    Callback onCcw;

    static void IRAM_ATTR registerTick(void *arg);
  };
};

#endif //ROTARYENCODER_H