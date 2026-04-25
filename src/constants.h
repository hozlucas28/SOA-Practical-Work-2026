#ifndef SRC_CONSTANTS_H_INCLUDED
#define SRC_CONSTANTS_H_INCLUDED

#include <LiquidCrystal_I2C.h>

#include "structs.h"

#define BUTTONS_DEBOUNCE_DELAY 25  // Miliseconds

#define LCD_ROWS 2
#define LCD_COLS 16

extern Button StockBtn;
extern Button SecurityBtn;

extern LiquidCrystal_I2C LCD;

extern WeightSensor WeightSensor01;
extern WeightSensor WeightSensor02;

#endif  // SRC_CONSTANTS_H_INCLUDED
