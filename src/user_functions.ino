
#include "constants.h"
#include "structs.h"
#include "user_functions.h"

// TODO: Utilizar FreeRTOS
void switchBtnState(Button* btn) {
    int btnRead = digitalRead(btn->pin);
    if (btnRead != btn->lastState) btn->lastDebounceTime = millis();

    if ((millis() - btn->lastDebounceTime) > btn->debounceDelay) {
        if (btnRead != btn->state) {
            btn->state = btnRead;

            if (btn->state == HIGH) {
                int ledRead = !digitalRead(btn->led);
                digitalWrite(btn->led, ledRead);
                btn->status = ledRead == HIGH ? ON : OFF;
            }
        }
    }

    btn->lastState = btnRead;
}

void lcdClear(LCD16x2* lcd) {
    lcd->device.clear();
    lcd->line01 = "";
    lcd->line02 = "";
}

void lcdPrint(LCD16x2* lcd, const String line) {
    if (lcd->line01 != line || lcd->line02 != "") lcd->device.clear();

    lcd->device.setCursor(0, 0);
    lcd->device.print(line);

    lcd->line01 = line;
    lcd->line02 = "";
}

void lcdPrint(LCD16x2* lcd, String line01, const String line02) {
    if (lcd->line01 != line01 || lcd->line02 != line02) lcd->device.clear();

    lcd->device.setCursor(0, 0);
    lcd->device.print(line01);

    lcd->device.setCursor(0, 1);
    lcd->device.print(line02);

    lcd->line01 = line01;
    lcd->line02 = line02;
}

unsigned int getWeightInGrams(WeightSensor* weightSensor) {
    if (!weightSensor->device.is_ready()) return 0;

    float currentWeight = weightSensor->device.get_units(10);

    return round(currentWeight);
}

void ledOn(WeightSensor* weightSensor) {
    if (!weightSensor->device.is_ready() || digitalRead(weightSensor->led) == HIGH) return;
    digitalWrite(weightSensor->led, HIGH);
}

void ledOff(WeightSensor* weightSensor) {
    if (!weightSensor->device.is_ready() || digitalRead(weightSensor->led) == LOW) return;
    digitalWrite(weightSensor->led, LOW);
}
