#ifndef GPIO_BUTTON_H
#define GPIO_BUTTON_H

#include "buttons.h"

namespace ModFirmWare
{

  class GPIOButton: public Buttons
  {
  public:
    GPIOButton(uint8_t pin, uint8_t mode=INPUT, bool reverse=false);

  protected:
    void setupButtons();
    uint16_t readButtons();

  private:
     uint8_t pin;
     uint8_t mode;
     bool inverted;
  };
  
};


#endif //GPIO_BUTTON_H