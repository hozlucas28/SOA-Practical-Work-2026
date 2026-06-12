#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include "enums.h"
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

/**
 * Period between iterations of `xMqttTask`, in milliseconds. Each tick services
 * `mqttClient.loop()` and publishes any state that changed since the last tick.
 */
#define MQTT_TASK_PERIOD_MS 100

/** Minimum delay between broker reconnection attempts, in milliseconds. */
#define MQTT_RECONNECT_INTERVAL_MS 5000

/**
 * MQTT keepalive, in seconds. Broker drops the client after ~1.5x this without
 * any packet. Kept generous because the Wokwi simulator runs slower than real
 * time, so `millis()`-based timing lags the broker's wall clock.
 */
#define MQTT_KEEPALIVE_S 30

/**
 * Period between forced "availability=online" heartbeats, in milliseconds.
 * Guarantees frequent outbound activity (well under the keepalive window) so the
 * broker never times the client out, even when the simulation clock is slow.
 */
#define MQTT_HEARTBEAT_MS 5000

/**
 * Time to wait for the persisted tare offsets (`tare/state` from Node-RED) after
 * requesting them on connect, in milliseconds. On timeout the boot-time tare
 * stands and the handshake is given up (retried on the next boot).
 */
#define TARE_RESPONSE_TIMEOUT_MS 4000

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

/**
 * Current FSM state. Written only by the FSM loop in `main.ino`; read from
 * `xMqttTask` to publish the device mode, hence `volatile`.
 */
extern volatile SystemStatus Status;

#endif  // CONSTANTS_H_INCLUDED
