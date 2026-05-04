#ifndef SRC_STRUCTS_H_INCLUDED
#define SRC_STRUCTS_H_INCLUDED

#include <Arduino.h>
#include <HX711.h>
#include <LiquidCrystal_I2C.h>

#include "enums.h"

// TODO: Agregar documentación
struct Button {
    const uint8_t pin;
    const uint8_t led;
    int state;
    ButtonStatus status;
    int lastState;
    const unsigned long debounceDelay;
    unsigned long lastDebounceTime;
};

// TODO: Agregar documentación
struct LCD16x2 {
    LiquidCrystal_I2C* device;
    String line01;
    String line02;
};

// TODO: Agregar documentación
struct Product {
    String name;
    unsigned int weight;
};

// TODO: Agregar documentación
struct WeightSensor {
    HX711 device;
    const uint8_t dtPin;
    const uint8_t sckPin;
    const uint8_t led;
    Product product;
    unsigned int baselineWeight;
    unsigned int minimumAcceptableStock;
};

#endif  // SRC_STRUCTS_H_INCLUDED
