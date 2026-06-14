#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include "enums.h"
#include "structs.h"

#define LCD_ROWS 2
#define LCD_COLS 16

#define WEIGHT_SENSOR_PRODUCT_NAME             "Apples"
#define WEIGHT_SENSOR_PRODUCT_WEIGHT           1000
#define WEIGHT_SENSOR_MINIMUM_ACCEPTABLE_STOCK 1

/**
 * @brief Calibration factor for the weight sensors.
 *
 * Current value represents the grams per unit of the raw sensor reading in Wokwi simulator.
 */
#define WEIGHT_SENSORS_CALIBRATION_FACTOR 0.42

/**
 * @brief Difference (in grams) above which a weight delta from the baseline is considered an anomaly in Security mode.
 */
#define ANOMALY_THRESHOLD 200

/**
 * @brief MQTT keepalive, in seconds.
 */
#define MQTT_KEEPALIVE 30

/**
 * @brief Delay (in milliseconds) between broker reconnection attempts.
 */
#define MQTT_RECONNECTION_DELAY 5000

/**
 * @brief Period between forced "availability=online" heartbeats, in milliseconds. Guarantees frequent outbound activity
 * (well under the keepalive window) so the broker never times the client out, even when the simulation clock is slow.
 */
#define MQTT_HEARTBEAT_MS 5000

/**
 * @brief Time to wait for the persisted tare offsets (`tare/state` from Node-RED) after requesting them on connect, in
 * milliseconds. On timeout the boot-time tare stands and the handshake is given up (retried on the next boot).
 */
#define TARE_RESPONSE_TIMEOUT_MS 4000

extern Button stockBtn;
extern Button securityBtn;

extern LCD16x2 LCD;

extern Buzzer buzzer;

extern WeightSensor weightSensor;

extern SystemStatus status;

#endif  // CONSTANTS_H_INCLUDED
