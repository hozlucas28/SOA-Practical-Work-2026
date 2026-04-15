#ifndef SRC_GLOBALS_H_INCLUDED
#define SRC_GLOBALS_H_INCLUDED

#include <LiquidCrystal_I2C.h>

#include "structs.h"

Button StockBtn;
Button SecurityBtn;

LiquidCrystal_I2C LCD;

WeightSensor WeightSensor01;
WeightSensor WeightSensor02;

#endif  // SRC_GLOBALS_H_INCLUDED
