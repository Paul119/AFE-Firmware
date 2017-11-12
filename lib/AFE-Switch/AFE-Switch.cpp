#include "AFE-Switch.h"

AFESwitch::AFESwitch(){};

AFESwitch::AFESwitch(uint8_t id) { begin(id); }

void AFESwitch::begin(uint8_t id) {
  AFEDataAccess Data;
  SwitchConfiguration = Data.getSwitchConfiguration(id);

  pinMode(SwitchConfiguration.gpio, INPUT_PULLUP);
  state = digitalRead(SwitchConfiguration.gpio);
  previousState = state;
}

boolean AFESwitch::getState() { return state; }

void AFESwitch::toggleState() { state = !state; }

boolean AFESwitch::isPressed() {
  if (pressed) {
    pressed = false;
    return true;
  } else {
    return false;
  }
}

boolean AFESwitch::is5s() {
  if (pressed4fiveSeconds) {
    pressed4fiveSeconds = false;
    return true;
  } else {
    return false;
  }
}

boolean AFESwitch::is10s() {
  if (pressed4tenSeconds) {
    pressed4tenSeconds = false;
    return true;
  } else {
    return false;
  }
}

void AFESwitch::listener() {

  boolean currentState = digitalRead(SwitchConfiguration.gpio);
  unsigned long time = millis();

  if (currentState != previousState) {

    if (startTime == 0) {
      startTime = time;
    }

    if (time - startTime >= SwitchConfiguration.sensitiveness) {
      if (SwitchConfiguration.type == SWITCH_TYPE_MONO) {

        if (!_pressed) {
          state = !state;
          _pressed = true;
          pressed = true;
        }

        if (time - startTime >= 10000 && !pressed4tenSeconds) {
          pressed4tenSeconds = true;
        }

      } else { // This is BI-stable code
        state = !state;
        previousState = currentState;
        pressed = true;
      }
    }
  } else if (currentState == previousState && startTime > 0) {
    if (SwitchConfiguration.type == SWITCH_TYPE_MONO &&
        time - startTime >= 5000 && time - startTime < 10000) {
      pressed4fiveSeconds = true;
    }

    startTime = 0;
    _pressed = false;
  }
}
