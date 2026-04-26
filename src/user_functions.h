#ifndef SRC_USER_FUNCTIONS_H_INCLUDED
#define SRC_USER_FUNCTIONS_H_INCLUDED

#include "structs.h"

void switchBtnState(Button* btn);

SystemEvent readStockBtn();

SystemEvent readStockSensors();

SystemEvent readSecurityBtn();

SystemEvent readAnomalySensors();

void lcdClear();

void lcdPrint(const String line01, const String line02);

void ledOn(const uint8_t pin);

void ledOff(const uint8_t pin);

#endif  // SRC_USER_FUNCTIONS_H_INCLUDED
