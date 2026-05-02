#ifndef SRC_USER_FUNCTIONS_H_INCLUDED
#define SRC_USER_FUNCTIONS_H_INCLUDED

#include "structs.h"

// TODO: Agregar documentación
void switchBtnState(Button* btn);

// TODO: Agregar documentación
unsigned int getWeightInGrams(WeightSensor* weightSensor);

// TODO: Agregar documentación
SystemEvent readStockBtn();

// TODO: Agregar documentación
SystemEvent readStockSensors();

// TODO: Agregar documentación
SystemEvent readSecurityBtn();

// TODO: Agregar documentación
SystemEvent readAnomalySensors();

// TODO: Agregar documentación
void lcdClear(LCD16x2* lcd);

// TODO: Agregar documentación
void lcdPrint(LCD16x2* lcd, const String line);
void lcdPrint(LCD16x2* lcd, const String line01, const String line02);

// TODO: Agregar documentación
void ledOn(const uint8_t pin);

// TODO: Agregar documentación
void ledOff(const uint8_t pin);

#endif  // SRC_USER_FUNCTIONS_H_INCLUDED
