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

// Event capture functions
#include "event_captures.h"

// Debugging utilities
#include "debuggers.h"

// FSM
SystemStatus Status = VIRGIN_EMBEDDED;

void handleEvent(SystemEvent event) {
    SystemStatus prevStatus = Status;

    switch (Status) {
        case VIRGIN_EMBEDDED:
            switch (event) {
                case STOCK_ON:
                    lcdClear(&LCD);
                    Status = STOCK_MODE;
                    DEBUG_FSM(VIRGIN_EMBEDDED, event, Status);
                    break;

                case SECURITY_ON:
                    lcdPrint(&LCD, "Security mode");
                    setBaselineWeight(&WeightSensor01);
                    setBaselineWeight(&WeightSensor02);
                    Status = SECURITY_MODE;
                    DEBUG_FSM(VIRGIN_EMBEDDED, event, Status);
                    break;

                default:
                    lcdPrint(&LCD, "SOA - Team L5", "S.S. control");
                    break;
            }
            break;

        case STOCK_MODE:
            switch (event) {
                case STOCK_OFF:
                    ledOff(&WeightSensor01);
                    ledOff(&WeightSensor02);
                    lcdClear(&LCD);
                    Status = VIRGIN_EMBEDDED;
                    DEBUG_FSM(STOCK_MODE, event, Status);
                    break;

                case STOCK_MISSING_SENSOR_01:
                    ledOn(&WeightSensor01);
                    ledOff(&WeightSensor02);
                    lcdPrint(&LCD, "Stock missing", "on sensor #01!");
                    DEBUG_FSM(STOCK_MODE, event, Status);
                    break;

                case STOCK_MISSING_SENSOR_02:
                    ledOff(&WeightSensor01);
                    ledOn(&WeightSensor02);
                    lcdPrint(&LCD, "Stock missing", "on sensor #02!");
                    DEBUG_FSM(STOCK_MODE, event, Status);
                    break;

                case STOCK_MISSING_SENSORS:
                    ledOn(&WeightSensor01);
                    ledOn(&WeightSensor02);
                    lcdPrint(&LCD, "Stock missing", "on all sensors!");
                    DEBUG_FSM(STOCK_MODE, event, Status);
                    break;

                case NO_MISSING_STOCK:
                    ledOff(&WeightSensor01);
                    ledOff(&WeightSensor02);
                    lcdPrint(
                        &LCD,
                        "Stock #01 = " + String(getStock(&WeightSensor01)),
                        "Stock #02 = " + String(getStock(&WeightSensor02))
                    );
                    break;

                case SECURITY_ON:
                    setBaselineWeight(&WeightSensor01);
                    setBaselineWeight(&WeightSensor02);
                    ledOff(&WeightSensor01);
                    ledOff(&WeightSensor02);
                    lcdPrint(&LCD, "Security mode");
                    Status = SECURITY_MODE;
                    DEBUG_FSM(STOCK_MODE, event, Status);
                    break;

                default:
                    break;
            }
            break;

        case SECURITY_MODE:
            switch (event) {
                case SECURITY_OFF:
                    stopBuzzer(&Alarm);
                    ledOff(&WeightSensor01);
                    ledOff(&WeightSensor02);
                    lcdClear(&LCD);
                    Status = VIRGIN_EMBEDDED;
                    DEBUG_FSM(SECURITY_MODE, event, Status);
                    break;

                case SECURITY_OFF_TO_STOCK:
                    stopBuzzer(&Alarm);
                    ledOff(&WeightSensor01);
                    ledOff(&WeightSensor02);
                    lcdClear(&LCD);
                    Status = STOCK_MODE;
                    DEBUG_FSM(SECURITY_MODE, event, Status);
                    break;

                case ANOMALY_SENSOR_01:
                    ledOn(&WeightSensor01);
                    playBuzzer(&Alarm);
                    lcdPrint(&LCD, "Security alert", "on sensor #01!");
                    DEBUG_FSM(SECURITY_MODE, event, Status);
                    break;

                case ANOMALY_SENSOR_02:
                    ledOn(&WeightSensor02);
                    playBuzzer(&Alarm);
                    lcdPrint(&LCD, "Security alert", "on sensor #02!");
                    DEBUG_FSM(SECURITY_MODE, event, Status);
                    break;

                case ANOMALY_SENSORS:
                    ledOn(&WeightSensor01);
                    ledOn(&WeightSensor02);
                    playBuzzer(&Alarm);
                    lcdPrint(&LCD, "Security alert", "on all sensors!");
                    DEBUG_FSM(SECURITY_MODE, event, Status);
                    break;

                default:
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
    pinMode(StockBtn.pin, INPUT);
    pinMode(StockBtn.led, OUTPUT);
    digitalWrite(StockBtn.led, LOW);

    StockBtn.state = HIGH;
    StockBtn.status = OFF;
    StockBtn.lastState = HIGH;
    StockBtn.lastDebounceTime = 0;

    DEBUG_BUTTON("StockBtn", StockBtn);
    DEBUG("\r\n");

    // Security button
    pinMode(SecurityBtn.pin, INPUT);
    pinMode(SecurityBtn.led, OUTPUT);
    digitalWrite(SecurityBtn.led, LOW);

    SecurityBtn.state = HIGH;
    SecurityBtn.status = OFF;
    SecurityBtn.lastState = HIGH;
    SecurityBtn.lastDebounceTime = 0;

    DEBUG_BUTTON("SecurityBtn", SecurityBtn);
    DEBUG("\r\n");

    // LCD
    LCD.device->begin(LCD_COLS, LCD_ROWS);
    LCD.device->setRGB(255, 255, 255);

    // Buzzer
    pinMode(Alarm.pin, OUTPUT);

    // Weight sensors
    WeightSensor01.device.begin(WeightSensor01.dtPin, WeightSensor01.sckPin);
    WeightSensor01.device.set_scale(WEIGHT_SENSORS_CALIBRATION_FACTOR);
    WeightSensor01.device.tare();

    pinMode(WeightSensor01.led, OUTPUT);

    DEBUG_WEIGHT_SENSOR("WeightSensor01", WeightSensor01);
    DEBUG("\r\n");

    WeightSensor02.device.begin(WeightSensor02.dtPin, WeightSensor02.sckPin);
    WeightSensor02.device.set_scale(WEIGHT_SENSORS_CALIBRATION_FACTOR);
    WeightSensor02.device.tare();

    pinMode(WeightSensor02.led, OUTPUT);

    DEBUG_WEIGHT_SENSOR("WeightSensor02", WeightSensor02);
    DEBUG("\r\n");

    // FreeRTOS tasks
    initSyncObjects();

    lockWeightSensors();
    sampleWeight(&WeightSensor01);
    sampleWeight(&WeightSensor02);
    unlockWeightSensors();

    xTaskCreate(xButtonsTask, "Buttons", 2048, NULL, 2, NULL);
    xTaskCreate(xWeightSampleTask, "WeightSample", 2048, NULL, 1, NULL);
    xTaskCreate(xBuzzerTask, "Alarm", 2048, &Alarm, 1, NULL);

    DEBUG("Setup completed.\r\n\n");
}

void loop() {
    handleEvent(getSecurityBtnEvent(Status));
    handleEvent(getAnomalySensorsEvent(Status));
    handleEvent(getStockBtnEvent(Status));
    handleEvent(getStockSensorsEvent(Status));
}
