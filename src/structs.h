#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

#include <Arduino.h>
#include <HX711.h>
#include <rgb_lcd.h>

#include "enums.h"

/**
 * @brief Button wrapper.
 *
 * Stores button configuration, current state, and debounce timing.
 */
struct Button {
    const uint8_t pin;                  ///< ESP32 pin connected to the button.
    const uint8_t ledPin;               ///< ESP32 pin connected to the status LED.
    int state;                          ///< Current electrical state.
    ButtonStatus status;                ///< Current status.
    int lastState;                      ///< Previous electrical state.
    const unsigned long debounceDelay;  ///< Debounce delay (in milliseconds) to prevent false triggers.
    unsigned long lastDebounceTime;     ///< Timestamp of the last state change for debounce logic.
};

struct LCD16x2 {
    rgb_lcd* device;
    String line01;
    String line02;
};

/**
 * @brief Product information.
 */
struct Product {
    String name;                          ///< Product name.
    unsigned int weight;                  ///< Weight of one unit of the product.
    unsigned int minimumAcceptableStock;  ///< Minimum acceptable stock (in units) for `STOCK_MODE`.
};

struct WeightSample {
    unsigned int weight;
    bool isValid;
};

/**
 * @brief HX711 load cell wrapper.
 *
 * Includes the HX711 instance, pin configuration, associated product information, baseline and minimum stock
 * thresholds, and the latest weight sample.
 */
struct WeightSensor {
    String id;                    ///< Unique identifier for the weight sensor.
    HX711 device;                 ///< HX711 instance.
    const uint8_t dtPin;          ///< ESP32 pin connected to the HX711 data line.
    const uint8_t sckPin;         ///< ESP32 pin connected to the HX711 serial clock line.
    const uint8_t ledPin;         ///< ESP32 pin connected to the status LED.
    Product product;              ///< Product information associated with the weight sensor.
    WeightSample sample;          ///< Latest weight sample reading.
    unsigned int baselineWeight;  ///< Baseline weight (in grams) for anomaly detection in `SECURITY_MODE`.
    bool anomaly;                 ///< Indicates whether an anomaly has been detected in `SECURITY_MODE`.
};

struct BuzzerStep {
    unsigned int frequency;
    unsigned long duration;
};

/**
 * @brief Buzzer control wrapper.
 *
 * Stores the output pin and the melody sequence to reproduce, plus its current playback state.
 */
struct Buzzer {
    const uint8_t pin;         ///< ESP32 pin connected to the buzzer.
    const BuzzerStep* steps;   ///< Melody steps.
    const size_t stepsLength;  ///< Number of steps in the melody sequence.
    bool muted;                ///< Indicates whether the buzzer is muted (no sound will be produced).
    bool playing;              ///< Indicates whether the buzzer is currently playing a melody.
};

#endif  // STRUCTS_H_INCLUDED
