#include "buttons.h"
#include "logengine.h"

#define LOGTAG "BUTTONS"

#define DEBOUNCE_TIME 30
#define LONG_PRESS_TIME 3000
#define DBL_CLICK_TIME 150

using namespace ModFirmWare;

Buttons::Buttons() : buttonPressCallback(nullptr) 
//*********************************************************************************
{
  logger = LogEngine::getInstance();
}


bool Buttons::setup(Application* app)
//*********************************************************************************
{

  if (Component::setup(app))
  {
    setupButtons();

    state = 0;
    eventState = 0;
    eventTime = 0;
    clicks = 0;

    return true;
  }
  else
  {
    return false;
  }
}

void Buttons::loop()
//*********************************************************************************
{
  
  uint16_t newState = readButtons();
  long t = millis();

  //logger->debug(LOGTAG, "nstate = %d, state = %d", newState, state);

  if ((newState != state) && (DEBOUNCE_TIME < (t - eventTime)))
  {
    //logger->debug(LOGTAG, "New State detected: %04X (old was %04X) at %d", newState, state, t);
    if (newState > state)
    {
      // button has been pressed start
      eventTime = t;
      state = newState;
      eventState = state;
    }
    else if (newState == 0)
    {
      // release
      unsigned long pressTime = t - eventTime;
      //logger->debug(LOGTAG, "Click counter before = %d", clicks);
      clicks += 1;

      if (LONG_PRESS_TIME <= pressTime)
      {
        eventTime = 0;
        longPress = true;
      }
      else
      {
        longPress = false;
        eventTime = (clicks > 1) ? 0 : t;
      }

      state = newState;
      //logger->debug(LOGTAG, "Click counter after = %d", clicks);
    }
    else
    {
      eventTime = t;
    }
  }

  if ((DBL_CLICK_TIME < (t - eventTime)) && (clicks > 0))
  {
    click_t type;
    if (longPress)
    {
      type = click_t::LONG;
    }
    else if (2 == clicks)
    {
      type = click_t::DOUBLE;
    }
    else
    {
      type = click_t::SINGLE;
    }

    //logger->info(LOGTAG, "\033[32m %s of button state x%04X \033[0m", 
    //   (type == click_t::LONG) ? "Long press" : (
    //   (type == click_t::DOUBLE) ? "Double click" : "Click"), eventState);
    // report the click
    if (buttonPressCallback != nullptr)
    {
      buttonPressCallback(eventState, type);
    }

    clicks = 0;
    eventTime = millis();
    eventState = 0;
  }
}

void Buttons::setButtonPressedCallBack(Callback cb)
//*********************************************************************************
{
  this->buttonPressCallback = cb;
}