
#include "constants.h"
#include "structs.h"
#include "sync.h"
#include "user_functions.h"

void switchBtn(Button* btn) {
    int read = digitalRead(btn->pin);
    if (read != btn->lastState) btn->lastDebounceTime = millis();

    if ((millis() - btn->lastDebounceTime) > btn->debounceDelay && read != btn->state) {
        btn->state = read;

        if (btn->state == HIGH) {
            ButtonStatus newStatus = btn->status == ON ? OFF : ON;
            btn->status = newStatus;
            digitalWrite(btn->led, newStatus);
        }
    }

    btn->lastState = read;
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

void lcdClear(LCD16x2* lcd) {
    lcd->device->clear();
    lcd->line01 = "";
    lcd->line02 = "";
}

void playBuzzer(Buzzer* buzzer) {
    lockBuzzer();
    if (!buzzer->playing) buzzer->playing = true;
    unlockBuzzer();
}

void stopBuzzer(Buzzer* buzzer) {
    lockBuzzer();
    buzzer->playing = false;
    unlockBuzzer();
}

unsigned int getWeight(WeightSensor* sensor) {
    unsigned int weight = 0;

    lockWeightSensor();
    if (sensor->sample.isValid) weight = sensor->sample.weight;
    unlockWeightSensor();

    return weight;
}

unsigned int getStock(WeightSensor* sensor) {
    unsigned int stock = 0;

    lockWeightSensor();
    if (sensor->sample.isValid) stock = sensor->sample.weight / sensor->product.weight;
    unlockWeightSensor();

    return stock;
}

void setOffset(WeightSensor* sensor, int32_t offset) {
    lockWeightSensor();
    sensor->device.set_offset(offset);
    unlockWeightSensor();
}

void setWeight(WeightSensor* sensor) {
    if (!sensor->device.is_ready()) {
        sensor->sample.isValid = false;
        return;
    }

    uint8_t times = 10;
    float weight = sensor->device.get_units(times);
    if (weight < 0.0f) weight = 0.0f;

    sensor->sample.weight = (unsigned int)floor(weight);
    sensor->sample.isValid = true;
}

void setBaselineWeight(WeightSensor* sensor) {
    lockWeightSensor();
    if (sensor->sample.isValid) sensor->baselineWeight = sensor->sample.weight;
    unlockWeightSensor();
}

int32_t tare(WeightSensor* sensor) {
    lockWeightSensor();
    sensor->device.tare();
    int32_t offset = sensor->device.get_offset();
    unlockWeightSensor();

    return offset;
}

void ledOn(WeightSensor* sensor) {
    if (digitalRead(sensor->led) == HIGH) return;
    digitalWrite(sensor->led, HIGH);
}

void ledOff(WeightSensor* sensor) {
    if (digitalRead(sensor->led) == LOW) return;
    digitalWrite(sensor->led, LOW);
}
