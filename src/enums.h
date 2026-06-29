#ifndef ENUMS_H_INCLUDED
#define ENUMS_H_INCLUDED

/**
 * @brief Logical button state.
 */
enum ButtonStatus {
    OFF,
    ON,
    UNKNOWN_BUTTON_STATUS,
};

/**
 * @brief Alarm status for the buzzer.
 */
enum AlarmStatus {
    MUTE,
    UNMUTE,
    UNKNOWN_ALARM_STATUS,
};

/**
 * @brief System status for the FSM.
 */
enum SystemStatus {
    VIRGIN_EMBEDDED,
    STOCK_MODE,
    SECURITY_MODE,
    UNKNOWN_SYSTEM_STATUS,
};

/**
 * @brief System event consumed by the FSM.
 */
enum SystemEvent {
    STOCK_ON,
    STOCK_OFF,
    STOCK_MISSING_SENSOR_01,
    NO_MISSING_STOCK,
    SECURITY_ON,
    SECURITY_OFF,
    SECURITY_OFF_TO_STOCK,
    ANOMALY_SENSOR_01,
    NO_ANOMALY,
    UNKNOWN_SYSTEM_EVENT,
};

/**
 * @brief Converts a string representation of a button status to the corresponding `ButtonStatus` enum value.
 *
 * @param status String representation of the button status (e.g., "ON" or "OFF").
 *
 * @return The corresponding `ButtonStatus` enum value. If the `status` does not match any known status, it returns
 * `UNKNOWN_BUTTON_STATUS`.
 */
const ButtonStatus buttonStatusToEnum(const char* status);

/**
 * @brief Converts a string representation of an alarm status to the corresponding `AlarmStatus` enum value.
 *
 * @param status String representation of the alarm status (e.g., "MUTE" or "UNMUTE").
 *
 * @return The corresponding `AlarmStatus` enum value. If the `status` does not match any known status, it returns
 * `UNKNOWN_ALARM_STATUS`.
 */
const AlarmStatus alarmStatusToEnum(const char* status);

/**
 * @brief Converts a `ButtonStatus` value to a human-readable string.
 *
 * @param status `ButtonStatus` value.
 *
 * @return A string representation of the `ButtonStatus`. If the `status` does not match any known status, it returns a
 * null pointer.
 */
const char* toString(ButtonStatus status);

/**
 * @brief Converts a `SystemStatus` value to a human-readable string.
 *
 * @param status `SystemStatus` value.
 *
 * @return A string representation of the `SystemStatus`. If the `status` does not match any known status, it returns a
 * null pointer.
 */
const char* toString(SystemStatus status);

#endif  // ENUMS_H_INCLUDED
