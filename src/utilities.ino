#include "structs.h"
#include "utilities.h"

void switchBtnState(Button *btn) {
  int btnRead = digitalRead(btn->pin);
  if (btnRead != btn->lastState) btn->lastDebounceTime = millis();

  if ((millis() - btn->lastDebounceTime) > btn->debounceDelay) {
    if (btnRead != btn->state) {
      btn->state = btnRead;

      if (btn->state == HIGH) {
        int ledRead = !digitalRead(btn->led.pin);
        digitalWrite(btn->led.pin, ledRead);
        btn->status = ledRead == HIGH ? ON : OFF;
      }
    }
  }

  btn->lastState = btnRead;
}
