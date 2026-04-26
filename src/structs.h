#ifndef SRC_STRUCTS_H_INCLUDED
#define SRC_STRUCTS_H_INCLUDED

#include <Arduino.h>
#include <HX711.h>

#include "enums.h"

struct Led {
    const uint8_t pin;
};

struct Button {
    const uint8_t pin;
    const Led led;
    int state;
    Status status;
    int lastState;
    const unsigned long debounceDelay;
    unsigned long lastDebounceTime;
};

struct WeightSensor {
    HX711 device;
    const uint8_t dtPin;
    const uint8_t sckPin;
    const Led led;
};

#endif  // SRC_STRUCTS_H_INCLUDED
