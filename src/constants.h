#ifndef SRC_CONSTANTS_H_INCLUDED
#define SRC_CONSTANTS_H_INCLUDED

#include "structs.h"

/** Debounce delay for the stock and security buttons, in milliseconds. */
#define BUTTONS_DEBOUNCE_DELAY 25

/** Period between button polling ticks in `xButtonsTask`, in milliseconds. */
#define BUTTONS_TASK_PERIOD_MS 5

/**
 * Period between weight samples in `xWeightSampleTask`, in milliseconds. Each
 * sample is the average of `WEIGHT_SENSORS_SAMPLES` raw HX711 reads, so the
 * task naturally pauses while the read is in flight.
 */
#define WEIGHT_SAMPLE_TASK_PERIOD_MS 200

/** Number of HX711 raw reads averaged per `get_units` call. */
#define WEIGHT_SENSORS_SAMPLES 10

/** Number of LCD rows. */
#define LCD_ROWS 2

/** Number of LCD columns. */
#define LCD_COLS 16

#define WEIGHT_SENSOR_PRODUCT_NAME             "Apples"
#define WEIGHT_SENSOR_PRODUCT_WEIGHT           1000
#define WEIGHT_SENSOR_MINIMUM_ACCEPTABLE_STOCK 1

/**
 * Calibration factor for the weight sensors.
 * Current value represents the grams per unit of the raw sensor reading.
 */
#define WEIGHT_SENSORS_CALIBRATION_FACTOR 0.42

/**
 * Difference (in grams) above which a weight delta from the baseline is
 * considered an anomaly in Security mode.
 */
#define ANOMALY_THRESHOLD 200

/** Push button that toggles Stock mode on/off. */
extern Button stockBtn;

/** Push button that toggles Security mode on/off. Has priority over Stock. */
extern Button securityBtn;

/** Shared 16x2 Grove RGB LCD instance. */
extern LCD16x2 LCD;

/** Buzzer used as Security-mode audible alarm. Driven by `xBuzzerTask`. */
extern Buzzer buzzer;

/** Load cell. Sampled by `xWeightSampleTask`. */
extern WeightSensor weightSensor;

#endif  // SRC_CONSTANTS_H_INCLUDED
