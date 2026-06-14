#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Defines (ESP32 pin's designations, structs, etc.)
#include "enums.h"
#include "pins.h"
#include "structs.h"

// Constants (global constants, structs, etc.)
#include "constants.h"

// User functions (utilities for sensors and actuators)
#include "user_functions.h"

// FreeRTOS tasks
#include "sync.h"
#include "tasks.h"

// MQTT integration
#include "mqtt.h"

// Event capture functions
#include "event_captures.h"

// Debugging utilities
#include "debuggers.h"

// FSM
SystemStatus status = VIRGIN_EMBEDDED;

void handleEvent(SystemEvent event) {
    SystemStatus prevStatus = status;

    switch (status) {
        case VIRGIN_EMBEDDED:
            switch (event) {
                case STOCK_ON:
                    lcdClear(&LCD);
                    status = STOCK_MODE;
                    DEBUG_FSM(VIRGIN_EMBEDDED, event, status);
                    break;

                case SECURITY_ON:
                    lcdPrint(&LCD, "Security mode");
                    setBaselineWeight(&weightSensor);
                    status = SECURITY_MODE;
                    DEBUG_FSM(VIRGIN_EMBEDDED, event, status);
                    break;

                default:
                    lcdPrint(&LCD, "SOA - Team L5", "S.S. control");
                    DEBUG_FSM(VIRGIN_EMBEDDED, event, VIRGIN_EMBEDDED);
                    break;
            }
            break;

        case STOCK_MODE:
            switch (event) {
                case STOCK_OFF:
                    ledOff(&weightSensor);
                    lcdClear(&LCD);
                    status = VIRGIN_EMBEDDED;
                    DEBUG_FSM(STOCK_MODE, event, status);
                    break;

                case STOCK_MISSING_SENSOR:
                    ledOn(&weightSensor);
                    lcdPrint(&LCD, "Stock missing", "on sensor!");
                    DEBUG_FSM(STOCK_MODE, event, status);
                    break;

                case NO_MISSING_STOCK:
                    ledOff(&weightSensor);
                    lcdPrint(&LCD, "Stock = " + String(getStock(&weightSensor)), "");
                    DEBUG_FSM(STOCK_MODE, event, STOCK_MODE);
                    break;

                case SECURITY_ON:
                    setBaselineWeight(&weightSensor);
                    ledOff(&weightSensor);
                    lcdPrint(&LCD, "Security mode");
                    status = SECURITY_MODE;
                    DEBUG_FSM(STOCK_MODE, event, status);
                    break;

                default:
                    break;
            }
            break;

        case SECURITY_MODE:
            switch (event) {
                case SECURITY_OFF:
                    stopBuzzer(&buzzer);
                    ledOff(&weightSensor);
                    lcdClear(&LCD);
                    status = VIRGIN_EMBEDDED;
                    DEBUG_FSM(SECURITY_MODE, event, status);
                    break;

                case SECURITY_OFF_TO_STOCK:
                    stopBuzzer(&buzzer);
                    ledOff(&weightSensor);
                    lcdClear(&LCD);
                    status = STOCK_MODE;
                    DEBUG_FSM(SECURITY_MODE, event, status);
                    break;

                case ANOMALY_SENSOR:
                    ledOn(&weightSensor);
                    playBuzzer(&buzzer);
                    lcdPrint(&LCD, "Security alert", "on all sensors!");
                    DEBUG_FSM(SECURITY_MODE, event, status);
                    break;

                default:
                    DEBUG_FSM(SECURITY_MODE, event, SECURITY_MODE);
                    break;
            }
            break;

        default:
            break;
    }
}

void setup() {
    Serial.begin(57600);

    DEBUG("\r\nStarting setup...\r\n\n");

    // Stock button
    pinMode(stockBtn.pin, INPUT);
    pinMode(stockBtn.led, OUTPUT);
    digitalWrite(stockBtn.led, LOW);

    stockBtn.state = HIGH;
    stockBtn.status = OFF;
    stockBtn.lastState = HIGH;
    stockBtn.lastDebounceTime = 0;

    DEBUG_BUTTON("stockBtn", stockBtn);
    DEBUG("\r\n");

    // Security button
    pinMode(securityBtn.pin, INPUT);
    pinMode(securityBtn.led, OUTPUT);
    digitalWrite(securityBtn.led, LOW);

    securityBtn.state = HIGH;
    securityBtn.status = OFF;
    securityBtn.lastState = HIGH;
    securityBtn.lastDebounceTime = 0;

    DEBUG_BUTTON("securityBtn", securityBtn);
    DEBUG("\r\n");

    // LCD
    LCD.device->begin(LCD_COLS, LCD_ROWS);
    LCD.device->setRGB(255, 255, 255);

    // Alarm
    pinMode(buzzer.pin, OUTPUT);

    // Weight sensor
    weightSensor.device.begin(weightSensor.dtPin, weightSensor.sckPin);
    weightSensor.device.set_scale(WEIGHT_SENSORS_CALIBRATION_FACTOR);
    weightSensor.device.tare();

    pinMode(weightSensor.led, OUTPUT);

    DEBUG_WEIGHT_SENSOR("weightSensor", weightSensor);
    DEBUG("\r\n");

    // FreeRTOS tasks
    initMutexs();

    lockWeightSensor();
    setWeight(&weightSensor);
    unlockWeightSensor();

    xTaskCreate(xButtonsTask, "Buttons", 2048, NULL, 2, NULL);
    xTaskCreate(xWeightSampleTask, "WeightSample", 2048, NULL, 1, NULL);
    xTaskCreate(xBuzzerTask, "Alarm", 2048, &buzzer, 1, NULL);
    xTaskCreate(xMQTTTask, "MQTT", 8192, NULL, 1, NULL);

    DEBUG("Setup completed.\r\n\n");
}

void loop() {
    handleEvent(getSecurityBtnEvent(status));
    handleEvent(getAnomalySensorEvent(status));
    handleEvent(getStockBtnEvent(status));
    handleEvent(getStockSensorEvent(status));
}
