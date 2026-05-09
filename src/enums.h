#ifndef SRC_ENUMS_H_INCLUDED
#define SRC_ENUMS_H_INCLUDED

/** Logical button state: ON when the user has it latched, OFF otherwise. */
enum ButtonStatus {
    OFF,
    ON,
};

/** FSM state. Only one is active at a time; Security wins over Stock. */
enum SystemStatus {
    VIRGIN_EMBEDDED,
    STOCK_MODE,
    SECURITY_MODE,
    UNKNOWN_SYSTEM_STATUS,
};

/** Event consumed by the FSM. Produced by the four capture functions. */
enum SystemEvent {
    STOCK_ON,
    STOCK_OFF,
    STOCK_MISSING_SENSOR_01,
    STOCK_MISSING_SENSOR_02,
    STOCK_MISSING_SENSORS,
    NO_MISSING_STOCK,
    SECURITY_ON,
    SECURITY_OFF,
    SECURITY_OFF_TO_STOCK,
    ANOMALY_SENSOR_01,
    ANOMALY_SENSOR_02,
    ANOMALY_SENSORS,
    NO_ANOMALY,
    UNKNOWN_SYSTEM_EVENT,
};

#endif  // SRC_ENUMS_H_INCLUDED
