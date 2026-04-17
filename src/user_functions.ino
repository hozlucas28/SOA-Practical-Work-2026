
#include "structs.h"
#include "constants.h"
#include "user_functions.h"

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

void lcdPrint(LCDMessage *message) {
  LCD.clear();

  if (message->row01.length() < LCD_COLS) {
    LCD.setCursor(0, 0);
    LCD.print(message->row01);
  }

  if (message->row02.length() < LCD_COLS) {
    LCD.setCursor(0, 1);
    LCD.print(message->row02);
  }
}
