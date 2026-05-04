#ifndef SRC_USER_FUNCTIONS_H_INCLUDED
#define SRC_USER_FUNCTIONS_H_INCLUDED

#include "enums.h"
#include "structs.h"

// TODO: Agregar documentación
void switchBtnState(Button* btn);

// TODO: Agregar documentación
void lcdClear(LCD16x2* lcd);

// TODO: Agregar documentación
void lcdPrint(LCD16x2* lcd, const String line);
void lcdPrint(LCD16x2* lcd, const String line01, const String line02);

// TODO: Agregar documentación
unsigned int getWeight(WeightSensor* weightSensor);

// TODO: Agregar documentación
unsigned int getStock(WeightSensor* weightSensor);

// TODO: Agregar documentación
void setBaselineWeight(WeightSensor* weightSensor);

// TODO: Agregar documentación
void ledOn(WeightSensor* weightSensor);

// TODO: Agregar documentación
void ledOff(WeightSensor* weightSensor);

#endif  // SRC_USER_FUNCTIONS_H_INCLUDED
