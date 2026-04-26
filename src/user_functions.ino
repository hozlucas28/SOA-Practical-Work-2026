
#include "constants.h"
#include "structs.h"
#include "user_functions.h"

void switchBtnState(Button* btn) {
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

SystemEvent readStockBtn() { return StockBtn.status == ON ? STOCK_ON : STOCK_OFF; }

SystemEvent readStockSensors() {
    // TODO
    return NO_MISSING_STOCK;
}

SystemEvent readSecurityBtn() { return SecurityBtn.status == ON ? SECURITY_ON : SECURITY_OFF; }

SystemEvent readAnomalySensors() {
    // TODO
    return SECURITY_OFF;
}

void lcdClear() { LCD.clear(); }

void lcdPrint(const String line01, const String line02) {
    LCD.setCursor(0, 0);
    LCD.print(line01);

    LCD.setCursor(0, 1);
    LCD.print(line02);
}

void ledOn(const uint8_t pin) { digitalWrite(pin, HIGH); }

void ledOff(const uint8_t pin) { digitalWrite(pin, LOW); }
