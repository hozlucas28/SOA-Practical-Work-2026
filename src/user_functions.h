#ifndef SRC_USER_FUNCTIONS_H_INCLUDED
#define SRC_USER_FUNCTIONS_H_INCLUDED

#include "structs.h"

static int baselineWeight01 = -1;
static int baselineWeight02 = -1;

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
void lcdClear();

// TODO: Agregar documentación
void lcdPrint(const String line01, const String line02);

// TODO: Agregar documentación
void ledOn(const uint8_t pin);

// TODO: Agregar documentación
void ledOff(const uint8_t pin);

const char* statusToString(SystemStatus s);

const char* eventToString(SystemEvent e);

void captureAnomalyBaseline();

void resetAnomalyBaseline();

#endif  // SRC_USER_FUNCTIONS_H_INCLUDED
