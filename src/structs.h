#ifndef SRC_STRUCTS_H_INCLUDED
#define SRC_STRUCTS_H_INCLUDED

#include <Arduino.h>
#include <HX711.h>

#include "enums.h"

struct Led {
  const int pin;
};

struct Button {
  const int pin;
  const Led led;
  int state;
  Status status;
  int lastState;
  const unsigned long debounceDelay;
  unsigned long lastDebounceTime;
};

struct LCDMessage {
  String row01;
  String row02;
};

struct WeightSensor {
  HX711 device;
  const int dtPin;
  const int sckPin;
  const Led led;
};

#endif  // SRC_STRUCTS_H_INCLUDED
