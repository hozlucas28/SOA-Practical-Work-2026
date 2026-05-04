
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
                btn->status = (btn->status == ON) ? OFF : ON;
                digitalWrite(btn->led, btn->status == ON ? HIGH : LOW);
            }
        }
    }

    btn->lastState = btnRead;
}

void sampleWeight(WeightSensor* weightSensor) {
    if (!weightSensor->device.is_ready()) {
        weightSensor->sample.valid = false;
        return;
    }

    float weight = weightSensor->device.get_units(WEIGHT_SENSORS_SAMPLES);

    // HX711 noise can dip slightly below zero near the tared baseline. Casting
    // a negative float to `unsigned int` wraps around to ~UINT_MAX, which then
    // makes `stock = weight / product.weight` huge and breaks both the stock
    // display and the "below minimum" check. Clamp before the cast.
    if (weight < 0.0f) weight = 0.0f;

    weightSensor->sample.weight = (unsigned int)floor(weight);
    weightSensor->sample.valid = true;
}

void lcdClear(LCD16x2* lcd) {
    lcd->device->clear();
    lcd->line01 = "";
    lcd->line02 = "";
}

void lcdPrint(LCD16x2* lcd, const String line) {
    if ((lcd->line01 != "" && lcd->line01 != line) || lcd->line02 != "") lcd->device->clear();

    lcd->device->setCursor(0, 0);
    lcd->device->print(line);

    lcd->line01 = line;
    lcd->line02 = "";
}

void lcdPrint(LCD16x2* lcd, const String line01, const String line02) {
    if ((lcd->line01 != "" && lcd->line01 != line01) || (lcd->line02 != "" && lcd->line02 != line02)) {
        lcd->device->clear();
    }

    lcd->device->setCursor(0, 0);
    lcd->device->print(line01);

    lcd->device->setCursor(0, 1);
    lcd->device->print(line02);

    lcd->line01 = line01;
    lcd->line02 = line02;
}

void applyTone(Buzzer* buzzer, unsigned int frequency) {
    if (frequency > 0) {
        tone(buzzer->pin, frequency);
    } else {
        noTone(buzzer->pin);
    };
}

void playBuzzer(Buzzer* buzzer) {
    if (!buzzer->playing) buzzer->playing = true;
}

void stopBuzzer(Buzzer* buzzer) {
    buzzer->playing = false;
}

unsigned int getWeight(WeightSensor* weightSensor) {
    if (!weightSensor->sample.valid) return 0;
    return weightSensor->sample.weight;
}

unsigned int getStock(WeightSensor* weightSensor) {
    if (!weightSensor->sample.valid) return 0;
    return weightSensor->sample.weight / weightSensor->product.weight;
}

bool tryGetWeight(WeightSensor* weightSensor, unsigned int* outWeight) {
    if (!weightSensor->sample.valid) return false;
    *outWeight = weightSensor->sample.weight;
    return true;
}

bool tryGetStock(WeightSensor* weightSensor, unsigned int* outStock) {
    if (!weightSensor->sample.valid) return false;
    *outStock = weightSensor->sample.weight / weightSensor->product.weight;
    return true;
}

void setBaselineWeight(WeightSensor* weightSensor) {
    if (!weightSensor->sample.valid) return;
    weightSensor->baselineWeight = weightSensor->sample.weight;
}

void ledOn(WeightSensor* weightSensor) {
    if (digitalRead(weightSensor->led) == HIGH) return;
    digitalWrite(weightSensor->led, HIGH);
}

void ledOff(WeightSensor* weightSensor) {
    if (digitalRead(weightSensor->led) == LOW) return;
    digitalWrite(weightSensor->led, LOW);
}
