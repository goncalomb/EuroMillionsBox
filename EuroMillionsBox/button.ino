#include "button.h"

int _button_timer = 0;
int _button_last_state = HIGH;

void button_setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

int button_get_state() {
  int button_state = digitalRead(BUTTON_PIN);
  int result = BUTTON_STATE_NONE;
  if (button_state == LOW) {
    if (_button_last_state == HIGH) { // button was just pressed
      _button_timer = millis();
    } else if (_button_timer != -1 && millis() - _button_timer > 1000) { // long-press (>1s)
      _button_timer = -1; // no more presses
      result = BUTTON_STATE_LONG_PRESS;
      Serial.println("button: long press");
    }
  } else if (_button_last_state == LOW) { // button was just released
    if (_button_timer != -1 && millis() - _button_timer > 50) { // short-press (>50ms <1s)
      result = BUTTON_STATE_SHORT_PRESS;
      Serial.println("button: short press");
    }
    _button_timer = 0;
  }
  _button_last_state = button_state;
  return result;
}
