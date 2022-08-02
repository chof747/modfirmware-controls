#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include <component.h>

namespace ModFirmWare
{

  class Buttons : public Component
  {
  public:
    enum click_t
    {
      SINGLE = 0,
      DOUBLE = 1,
      LONG = 2
    };

    typedef std::function<void(const uint16_t state, click_t type)> onButtonPressedCallBackType;

    Buttons() : buttonPressCallback(nullptr) {}

    bool setup();
    void loop();

    void setButtonPressedCallBack(onButtonPressedCallBackType cb);

  protected:
    virtual void setupButtons() = 0;
    virtual uint16_t readButtons() = 0;

  private:
    uint16_t state;
    uint16_t eventState;
    unsigned long eventTime;
    uint8_t clicks;
    bool longPress;

    onButtonPressedCallBackType buttonPressCallback;
  };

};

#endif // BUTTONS_H
