#ifndef SRC_CONSTANTS_H_INCLUDED
#define SRC_CONSTANTS_H_INCLUDED

#include <LiquidCrystal_I2C.h>

#include "structs.h"

/**
 * Debounce delay for the stock and security buttons, in milliseconds.
 * */
#define BUTTONS_DEBOUNCE_DELAY 25

#define LCD_ROWS 2
#define LCD_COLS 16

#define WEIGHT_SENSOR_01_PRODUCT_NAME             "Apples"
#define WEIGHT_SENSOR_01_PRODUCT_WEIGHT           1000
#define WEIGHT_SENSOR_01_MINIMUM_ACCEPTABLE_STOCK 1

#define WEIGHT_SENSOR_02_PRODUCT_NAME             "Cookies"
#define WEIGHT_SENSOR_02_PRODUCT_WEIGHT           500
#define WEIGHT_SENSOR_02_MINIMUM_ACCEPTABLE_STOCK 3

/**
 * Calibration factor for the weight sensors.
 * Current value represents the grams per unit of the raw sensor reading.
 */
#define WEIGHT_SENSORS_CALIBRATION_FACTOR 0.42

/**
 * Difference to consider an anomaly in the weight sensor readings.
 * Current value is represented in grams.
 */
#define ANOMALY_THRESHOLD 200

/* TODO: Agregar documentación */
extern Button StockBtn;

/* TODO: Agregar documentación */
extern Button SecurityBtn;

/* TODO: Agregar documentación */
extern LCD16x2 LCD;

/* TODO: Agregar documentación */
extern WeightSensor WeightSensor01;

/* TODO: Agregar documentación */
extern WeightSensor WeightSensor02;

#endif  // SRC_CONSTANTS_H_INCLUDED
