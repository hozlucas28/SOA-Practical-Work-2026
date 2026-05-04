#ifndef SRC_DEBUGGERS_H_INCLUDED
#define SRC_DEBUGGERS_H_INCLUDED

#include "enums.h"

#define DEBUG_MODE true

#if DEBUG_MODE

/* TODO: Agregar documentación */
#define DEBUG(message, ...) Serial.printf(message, ##__VA_ARGS__)

/* TODO: Agregar documentación */
#define DEBUG_SIGNAL(signal, printMethod) printMethod(signal ? "HIGH" : "LOW")

/* TODO: Agregar documentación */
#define DEBUG_BUTTON_STATUS(status, printMethod)  \
    switch (status) {                             \
        case ON:                                  \
            printMethod("ON");                    \
            break;                                \
                                                  \
        case OFF:                                 \
            printMethod("OFF");                   \
            break;                                \
                                                  \
        default:                                  \
            printMethod("UNKNOWN_BUTTON_STATUS"); \
    }

/* TODO: Agregar documentación */
#define DEBUG_BUTTON(name, button)                                            \
    Serial.printf("%s.pin = %u\r\n", name, button.pin);                       \
    Serial.printf("%s.led = %u\r\n", name, button.led);                       \
    Serial.printf("%s.state = ", name);                                       \
    DEBUG_SIGNAL(button.state, Serial.println);                               \
    Serial.printf("%s.status = ", name);                                      \
    DEBUG_BUTTON_STATUS(button.status, Serial.println);                       \
    Serial.printf("%s.lastState = ", name);                                   \
    DEBUG_SIGNAL(button.lastState, Serial.println);                           \
    Serial.printf("%s.debounceDelay = %ums\r\n", name, button.debounceDelay); \
    Serial.printf("%s.lastDebounceTime = %ums\r\n", name, button.lastDebounceTime)

/* TODO: Agregar documentación */
#define DEBUG_WEIGHT_SENSOR(name, sensor)                     \
    Serial.printf("%s.device = %p\r\n", name, sensor.device); \
    Serial.printf("%s.dtPin = %u\r\n", name, sensor.dtPin);   \
    Serial.printf("%s.sckPin = %u\r\n", name, sensor.sckPin); \
    Serial.printf("%s.led = %u\r\n", name, sensor.led)

/* TODO: Agregar documentación */
#define DEBUG_SYSTEM_STATUS(status, printMethod)  \
    switch (status) {                             \
        case VIRGIN_EMBEDDED:                     \
            printMethod("VIRGIN_EMBEDDED      "); \
            break;                                \
                                                  \
        case STOCK_MODE:                          \
            printMethod("STOCK_MODE           "); \
            break;                                \
                                                  \
        case SECURITY_MODE:                       \
            printMethod("SECURITY_MODE        "); \
            break;                                \
                                                  \
        default:                                  \
            printMethod("UNKNOWN_SYSTEM_STATUS"); \
    }

/* TODO: Agregar documentación */
#define DEBUG_SYSTEM_EVENT(event, printMethod)      \
    switch (event) {                                \
        case STOCK_ON:                              \
            printMethod("STOCK_ON               "); \
            break;                                  \
                                                    \
        case STOCK_OFF:                             \
            printMethod("STOCK_OFF              "); \
            break;                                  \
                                                    \
        case STOCK_MISSING_SENSOR_01:               \
            printMethod("STOCK_MISSING_SENSOR_01"); \
            break;                                  \
                                                    \
        case STOCK_MISSING_SENSOR_02:               \
            printMethod("STOCK_MISSING_SENSOR_02"); \
            break;                                  \
                                                    \
        case STOCK_MISSING_SENSORS:                 \
            printMethod("STOCK_MISSING_SENSORS  "); \
            break;                                  \
                                                    \
        case NO_MISSING_STOCK:                      \
            printMethod("NO_MISSING_STOCK       "); \
            break;                                  \
                                                    \
        case SECURITY_ON:                           \
            printMethod("SECURITY_ON            "); \
            break;                                  \
                                                    \
        case SECURITY_OFF:                          \
            printMethod("SECURITY_OFF           "); \
            break;                                  \
                                                    \
        case ANOMALY_SENSOR_01:                     \
            printMethod("ANOMALY_SENSOR_01      "); \
            break;                                  \
                                                    \
        case ANOMALY_SENSOR_02:                     \
            printMethod("ANOMALY_SENSOR_02      "); \
            break;                                  \
                                                    \
        case ANOMALY_SENSORS:                       \
            printMethod("ANOMALY_SENSORS        "); \
            break;                                  \
                                                    \
        default:                                    \
            printMethod("UNKNOWN_SYSTEM_EVENT   "); \
    }

/* TODO: Agregar documentación */
#define DEBUG_FSM(status, event, nextStatus)   \
    DEBUG_SYSTEM_STATUS(status, Serial.print); \
    DEBUG_SYSTEM_EVENT(event, Serial.print);   \
    Serial.print(" --> ");                     \
    DEBUG_SYSTEM_STATUS(nextStatus, Serial.println)

#else

#define DEBUG(message, ...)                      ;
#define DEBUG_SIGNAL(signal, printMethod)        ;
#define DEBUG_BUTTON_STATUS(status)              ;
#define DEBUG_BUTTON(name, button)               ;
#define DEBUG_WEIGHT_SENSOR(name, sensor)        ;
#define DEBUG_SYSTEM_STATUS(status, printMethod) ;
#define DEBUG_SYSTEM_EVENT(event, printMethod)   ;
#define DEBUG_FSM(status, event, nextStatus)     ;

#endif
#endif  // SRC_DEBUGGERS_H_INCLUDED
