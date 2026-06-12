#ifndef ENUMS_H_INCLUDED
#define ENUMS_H_INCLUDED

/**
 * Logical button state.
 */
enum ButtonStatus {
    OFF,
    ON,
};

/**
 * System status for the FSM.
 */
enum SystemStatus {
    VIRGIN_EMBEDDED,
    STOCK_MODE,
    SECURITY_MODE,
    UNKNOWN_SYSTEM_STATUS,
};

/**
 * System event consumed by the FSM.
 */
enum SystemEvent {
    STOCK_ON,
    STOCK_OFF,
    STOCK_MISSING_SENSOR,
    NO_MISSING_STOCK,
    SECURITY_ON,
    SECURITY_OFF,
    SECURITY_OFF_TO_STOCK,
    ANOMALY_SENSOR,
    NO_ANOMALY,
    UNKNOWN_SYSTEM_EVENT,
};

#endif  // ENUMS_H_INCLUDED
