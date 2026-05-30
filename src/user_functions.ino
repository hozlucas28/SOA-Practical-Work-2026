
#include "constants.h"
#include "structs.h"
#include "sync.h"
#include "user_functions.h"

void switchBtnState(Button* btn) {
    int btnRead = digitalRead(btn->pin);
    if (btnRead != btn->lastState) btn->lastDebounceTime = millis();

    if ((millis() - btn->lastDebounceTime) > btn->debounceDelay) {
        if (btnRead != btn->state) {
            btn->state = btnRead;

            if (btn->state == HIGH) {
                applyButtonStatus(btn, (btn->status == ON) ? OFF : ON);
            }
        }
    }

    btn->lastState = btnRead;
}

void applyButtonStatus(Button* btn, ButtonStatus status) {
    btn->status = status;
    digitalWrite(btn->led, status == ON ? HIGH : LOW);
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
    lockBuzzer();

    if (!buzzer->playing) buzzer->playing = true;

    unlockBuzzer();
}

void stopBuzzer(Buzzer* buzzer) {
    lockBuzzer();

    buzzer->playing = false;

    unlockBuzzer();
}

// Remote mute flag for the alarm. Written from the MQTT `cmd/alarm` handler
// (xMqttTask) and read by `triggerAlarm` from the FSM loop, hence `volatile`.
static volatile bool AlarmMuted = false;

void setAlarmMuted(bool muted) {
    AlarmMuted = muted;
}

void triggerAlarm(Buzzer* buzzer) {
    if (!AlarmMuted) playBuzzer(buzzer);
}

void silenceAlarm(Buzzer* buzzer) {
    stopBuzzer(buzzer);
    setAlarmMuted(false);
}

unsigned int getWeight(WeightSensor* weightSensor) {
    unsigned int weight = 0;

    lockWeightSensors();

    if (weightSensor->sample.valid) weight = weightSensor->sample.weight;

    unlockWeightSensors();

    return weight;
}

unsigned int getStock(WeightSensor* weightSensor) {
    unsigned int stock = 0;

    lockWeightSensors();

    if (weightSensor->sample.valid) {
        stock = weightSensor->sample.weight / weightSensor->product.weight;
    }

    unlockWeightSensors();

    return stock;
}

bool tryGetWeight(WeightSensor* weightSensor, unsigned int* outWeight) {
    bool valid = false;

    lockWeightSensors();

    if (weightSensor->sample.valid) {
        *outWeight = weightSensor->sample.weight;
        valid = true;
    }

    unlockWeightSensors();

    return valid;
}

bool tryGetStock(WeightSensor* weightSensor, unsigned int* outStock) {
    bool valid = false;

    lockWeightSensors();

    if (weightSensor->sample.valid) {
        *outStock = weightSensor->sample.weight / weightSensor->product.weight;
        valid = true;
    }

    unlockWeightSensors();

    return valid;
}

void setBaselineWeight(WeightSensor* weightSensor) {
    lockWeightSensors();

    if (weightSensor->sample.valid) {
        weightSensor->baselineWeight = weightSensor->sample.weight;
    }

    unlockWeightSensors();
}

void ledOn(WeightSensor* weightSensor) {
    if (digitalRead(weightSensor->led) == HIGH) return;
    digitalWrite(weightSensor->led, HIGH);
}

void ledOff(WeightSensor* weightSensor) {
    if (digitalRead(weightSensor->led) == LOW) return;
    digitalWrite(weightSensor->led, LOW);
}
