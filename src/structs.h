#ifndef SRC_STRUCTS_H_INCLUDED
#define SRC_STRUCTS_H_INCLUDED

#include <Arduino.h>
#include <HX711.h>
#include <LiquidCrystal_I2C.h>

#include "enums.h"

/**
 * Push button with a paired status LED. Mutable fields are `volatile` because
 * they are written from the buttons FreeRTOS task and read from the FSM loop
 * and event captures.
 */
struct Button {
    const uint8_t pin;
    const uint8_t led;
    volatile int state;
    volatile ButtonStatus status;
    volatile int lastState;
    const unsigned long debounceDelay;
    volatile unsigned long lastDebounceTime;
};

/**
 * I2C 16x2 LCD wrapper. `line01` and `line02` cache the last printed text so
 * `lcdPrint` can skip a full clear when the content is unchanged.
 */
struct LCD16x2 {
    LiquidCrystal_I2C* device;
    String line01;
    String line02;
};

/**
 * Product placed on a shelf. `weight` is the per-unit mass in grams used to
 * convert raw scale readings into stock counts.
 */
struct Product {
    String name;
    unsigned int weight;
};

/**
 * Latest sampled weight from a `WeightSensor`. Produced by
 * `xWeightSampleTask`, consumed by the FSM and event captures. `valid` is
 * cleared while the HX711 is unready so consumers can skip stale data
 * instead of treating it as a real `0 g` reading.
 */
struct WeightSample {
    volatile unsigned int weight;
    volatile bool valid;
};

/**
 * HX711 load cell wrapper with cached sample, baseline (used by Security
 * mode anomaly detection) and minimum acceptable stock (used by Stock mode
 * shortage detection).
 */
struct WeightSensor {
    HX711 device;
    const uint8_t dtPin;
    const uint8_t sckPin;
    const uint8_t led;
    Product product;
    unsigned int baselineWeight;
    unsigned int minimumAcceptableStock;
    WeightSample sample;
};

/** Single tone step in a `Buzzer` melody: frequency in Hz, duration in ms. */
struct BuzzerStep {
    unsigned int frequency;
    unsigned long duration;
};

/**
 * Buzzer driven by a dedicated FreeRTOS task. `playing` is the only
 * cross-core signal: it is `volatile` because the FSM toggles it from one
 * core and `xBuzzerTask` reads it from another.
 */
struct Buzzer {
    const uint8_t pin;
    const BuzzerStep* steps;
    const size_t stepsLength;
    volatile bool playing;
};

#endif  // SRC_STRUCTS_H_INCLUDED
