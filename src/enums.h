#ifndef SRC_ENUMS_H_INCLUDED
#define SRC_ENUMS_H_INCLUDED

/* TODO: Agregar documentación */
enum ButtonStatus {
    OFF,
    ON,
};

/* TODO: Agregar documentación */
enum SystemStatus {
    VIRGIN_EMBEDDED,
    STOCK_MODE,
    SECURITY_MODE,
    UNKNOWN_SYSTEM_STATUS,
};

/* TODO: Agregar documentación */
enum SystemEvent {
    STOCK_ON,
    STOCK_OFF,
    STOCK_MISSING_SENSOR_01,
    STOCK_MISSING_SENSOR_02,
    STOCK_MISSING_SENSORS,
    NO_MISSING_STOCK,
    SECURITY_ON,
    SECURITY_OFF,
    ANOMALY_SENSOR_01,
    ANOMALY_SENSOR_02,
    ANOMALY_SENSORS,
    NO_ANOMALY,
    UNKNOWN_SYSTEM_EVENT,
};

#endif  // SRC_ENUMS_H_INCLUDED
