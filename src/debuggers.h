#ifndef DEBUGGERS_H_INCLUDED
#define DEBUGGERS_H_INCLUDED

#include "enums.h"

#define DEBUG_MODE false

#if DEBUG_MODE

/**
 * @brief Prints a formatted debug message to the serial monitor.
 * @param message The format string.
 * @param ... Optional arguments to format into the message.
 */
#define DEBUG(message, ...) Serial.printf(message, ##__VA_ARGS__);

/**
 * @brief Prints a signal value (e.g. `Button.state`) as `HIGH`, or `LOW`.
 * @param signal The signal value.
 * @param printMethod The method to use for printing (e.g. `Serial.println`).
 */
#define DEBUG_SIGNAL(signal, printMethod) printMethod(signal ? "HIGH" : "LOW");

/**
 * @brief Prints each field of a `Button` to the serial monitor.
 * @param name The name of the button (e.g. "button1") to prefix each field with.
 * @param button The `Button` instance.
 */
#define DEBUG_BUTTON(name, button)                                            \
    Serial.printf("%s.pin = %u\r\n", name, button.pin);                       \
    Serial.printf("%s.ledPin = %u\r\n", name, button.ledPin);                 \
    Serial.printf("%s.state = ", name);                                       \
    DEBUG_SIGNAL(button.state, Serial.println);                               \
    Serial.printf("%s.status = \r\n", name, toString(button.status));         \
    Serial.printf("%s.lastState = ", name);                                   \
    DEBUG_SIGNAL(button.lastState, Serial.println);                           \
    Serial.printf("%s.debounceDelay = %ums\r\n", name, button.debounceDelay); \
    Serial.printf("%s.lastDebounceTime = %ums\r\n", name, button.lastDebounceTime);

/**
 * @brief Prints each field of a `WeightSensor` to the serial monitor.
 * @param name The name of the weight sensor (e.g. "weightSensor01") to prefix each field with.
 * @param sensor The `WeightSensor` instance.
 */
#define DEBUG_WEIGHT_SENSOR(name, sensor)                     \
    Serial.printf("%s.id = %s\r\n", name, sensor.id.c_str()); \
    Serial.printf("%s.device = %p\r\n", name, sensor.device); \
    Serial.printf("%s.dtPin = %u\r\n", name, sensor.dtPin);   \
    Serial.printf("%s.sckPin = %u\r\n", name, sensor.sckPin); \
    Serial.printf("%s.ledPin = %u\r\n", name, sensor.ledPin);

/**
 * @brief
 */
#define DEBUG_WIFI()                                                                        \
    if (WiFi.status() == WL_CONNECTED) {                                                    \
        Serial.printf("WiFi connected (local IP: %s).", WiFi.localIP().toString().c_str()); \
    } else {                                                                                \
        Serial.println("WiFi not connected.");                                              \
    }

/**
 * @brief Prints a `SystemStatus` value.
 * @param status The `SystemStatus` value.
 * @param printMethod The method to use for printing (e.g. `Serial.println`).
 */
#define DEBUG_SYSTEM_STATUS(status, printMethod)  \
    switch (status) {                             \
        case SystemStatus::VIRGIN_EMBEDDED:       \
            printMethod("VIRGIN_EMBEDDED      "); \
            break;                                \
                                                  \
        case SystemStatus::STOCK_MODE:            \
            printMethod("STOCK_MODE           "); \
            break;                                \
                                                  \
        case SystemStatus::SECURITY_MODE:         \
            printMethod("SECURITY_MODE        "); \
            break;                                \
                                                  \
        default:                                  \
            printMethod("UNKNOWN_SYSTEM_STATUS"); \
    }

/**
 * @brief Prints a `SystemEvent` value.
 * @param event The `SystemEvent` value.
 * @param printMethod The method to use for printing (e.g. `Serial.println`).
 */
#define DEBUG_SYSTEM_EVENT(event, printMethod)         \
    switch (event) {                                   \
        case SystemEvent::STOCK_ON:                    \
            printMethod("STOCK_ON               ");    \
            break;                                     \
                                                       \
        case SystemEvent::STOCK_OFF:                   \
            printMethod("STOCK_OFF              ");    \
            break;                                     \
                                                       \
        case SystemEvent::STOCK_MISSING_SENSOR_01:     \
            printMethod("STOCK_MISSING_SENSOR_01   "); \
            break;                                     \
                                                       \
        case SystemEvent::NO_MISSING_STOCK:            \
            printMethod("NO_MISSING_STOCK       ");    \
            break;                                     \
                                                       \
        case SystemEvent::SECURITY_ON:                 \
            printMethod("SECURITY_ON            ");    \
            break;                                     \
                                                       \
        case SystemEvent::SECURITY_OFF:                \
            printMethod("SECURITY_OFF           ");    \
            break;                                     \
                                                       \
        case SystemEvent::SECURITY_OFF_TO_STOCK:       \
            printMethod("SECURITY_OFF_TO_STOCK  ");    \
            break;                                     \
                                                       \
        case SystemEvent::ANOMALY_SENSOR_01:           \
            printMethod("ANOMALY_SENSOR_01         "); \
            break;                                     \
                                                       \
        default:                                       \
            printMethod("UNKNOWN_SYSTEM_EVENT   ");    \
    }

/**
 * @brief Prints a FSM transition as `<systemStatus> <event> --> <nextSystemStatus>`.
 * @param status The current `SystemStatus` value.
 * @param event The `SystemEvent` value that triggers the transition.
 * @param nextStatus The next `SystemStatus` value after the transition.
 */
#define DEBUG_FSM(status, event, nextStatus)   \
    DEBUG_SYSTEM_STATUS(status, Serial.print); \
    DEBUG_SYSTEM_EVENT(event, Serial.print);   \
    Serial.print(" --> ");                     \
    DEBUG_SYSTEM_STATUS(nextStatus, Serial.println);

#else

#define DEBUG(message, ...)                      ;
#define DEBUG_SIGNAL(signal, printMethod)        ;
#define DEBUG_BUTTON(name, button)               ;
#define DEBUG_WEIGHT_SENSOR(name, sensor)        ;
#define DEBUG_WIFI()                             ;
#define DEBUG_SYSTEM_STATUS(status, printMethod) ;
#define DEBUG_SYSTEM_EVENT(event, printMethod)   ;
#define DEBUG_FSM(status, event, nextStatus)     ;

#endif  // DEBUG_MODE
#endif  // DEBUGGERS_H_INCLUDED
