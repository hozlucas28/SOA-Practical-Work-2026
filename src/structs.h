#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

#include <Arduino.h>
#include <HX711.h>
#include <rgb_lcd.h>

#include "enums.h"

/**
 * Button wrapper. Stores button configuration, current state,
 * and debounce timing.
 */
struct Button {
    /** ESP32 pin connected to the button. */
    const uint8_t pin;
    /** ESP32 pin connected to the status LED. */
    const uint8_t led;
    /** Current electrical state. */
    int state;
    /** Current status. */
    ButtonStatus status;
    /** Previous electrical state. */
    int lastState;
    /** Debounce delay (in milliseconds) to prevent false triggers. */
    const unsigned long debounceDelay;
    /** Timestamp of the last state change for debounce logic. */
    unsigned long lastDebounceTime;
};

struct LCD16x2 {
    rgb_lcd* device;
    String line01;
    String line02;
};

/**
 * Product information.
 */
struct Product {
    /** Product name. */
    String name;
    /** Weight of one unit of the product. */
    unsigned int weight;
};

struct WeightSample {
    unsigned int weight;
    bool isValid;
};

/**
 * HX711 load cell wrapper with cached sample, baseline (used by `SECURITY_MODE`) and minimum
 * acceptable stock (used by `STOCK_MODE`).
 */
struct WeightSensor {
    /** HX711 instance. */
    HX711 device;
    /** ESP32 pin connected to the HX711 data line. */
    const uint8_t dtPin;
    /** ESP32 pin connected to the HX711 serial clock line. */
    const uint8_t sckPin;
    /** ESP32 pin connected to the status LED. */
    const uint8_t led;
    /** Product information associated with the weight sensor. */
    Product product;
    /** Baseline weight (in grams) for anomaly detection in `SECURITY_MODE`. */
    unsigned int baselineWeight;
    /** Minimum acceptable stock (in grams) for `STOCK_MODE`. */
    unsigned int minimumAcceptableStock;
    /** Latest weight sample reading. */
    WeightSample sample;
};

struct BuzzerStep {
    unsigned int frequency;
    unsigned long duration;
};

/**
 * Buzzer control wrapper. Stores the output pin and the melody sequence to
 * reproduce, plus its current playback state.
 */
struct Buzzer {
    /** ESP32 pin connected to the buzzer. */
    const uint8_t pin;
    /** Melody steps. */
    const BuzzerStep* steps;
    /** Number of steps in the melody sequence. */
    const size_t stepsLength;
    /** Indicates whether the buzzer is currently playing a melody. */
    bool playing;
};

#endif  // STRUCTS_H_INCLUDED
