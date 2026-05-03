#ifndef SRC_CONSTANTS_H_INCLUDED
#define SRC_CONSTANTS_H_INCLUDED

#include <LiquidCrystal_I2C.h>

#include "structs.h"

#define BUTTONS_DEBOUNCE_DELAY 25  // Miliseconds.

#define LCD_ROWS 2
#define LCD_COLS 16

#define WEIGHT_SENSORS_CALIBRATION_FACTOR 0.42  // Grams.

#define STOCK_MIN_WEIGHT  200  // gramos mínimos para considerar que hay stock
#define ANOMALY_THRESHOLD 150  // variación en gramos para considerar anomalía

/* TODO: Agregar documentación */
extern Button StockBtn;

/* TODO: Agregar documentación */
extern Button SecurityBtn;

/* TODO: Agregar documentación */
extern LiquidCrystal_I2C LCD;

/* TODO: Agregar documentación */
extern WeightSensor WeightSensor01;

/* TODO: Agregar documentación */
extern WeightSensor WeightSensor02;

#endif  // SRC_CONSTANTS_H_INCLUDED
