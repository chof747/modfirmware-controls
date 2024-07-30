#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include <modfw_component.h>

namespace ModFirmWare
{

  class LogEngine;

  class Buttons : public Component
  {
  public:
    enum click_t
    {
      SINGLE = 0,
      DOUBLE = 1,
      LONG = 2
    };

     using Callback = std::function<void(const uint16_t state, click_t type)>;

    Buttons();

    bool setup(Application*);
    void loop();

    void setButtonPressedCallBack(Callback cb);

  protected:
    LogEngine* logger;
    virtual void setupButtons() = 0;
    virtual uint16_t readButtons() = 0;

  private:
    uint16_t state;
    uint16_t eventState;
    unsigned long eventTime;
    uint8_t clicks;
    bool longPress;

    Callback buttonPressCallback;
  };

};

#endif // BUTTONS_H
