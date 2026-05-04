
#include <Arduino.h>

// Defines (ESP32 pin's designations, structs, etc.)
#include "enums.h"
#include "pins.h"
#include "structs.h"

// Constants (global constants, structs, etc.)
#include "constants.h"

// User functions (utilities for sensors and actuators)
#include "user_functions.h"

// Event capture functions
#include "event_captures.h"

// Debugging utilities
#include "debuggers.h"

////////////////////////////////////////////////////
// TODO: Utilizar FreeRTOS para manejar la melodia del Buzzer.
struct BuzzerStep {
    unsigned int frequency;
    unsigned long duration;
};

const BuzzerStep BuzzerSteps[] = {
    { .frequency = 700, .duration = 500 },
    { .frequency = 560, .duration = 500 },
    { .frequency = 700, .duration = 500 },
    { .frequency = 560, .duration = 500 },
};

const int BuzzerStepsLength = sizeof(BuzzerSteps) / sizeof(BuzzerSteps[0]);

// 3. Variables de estado (deben ser globales o estáticas)
int pasoActual = 0;
unsigned long tiempoAnterior = 0;
bool alarmaIniciada = false;

void playBuzzer() {
    unsigned long currentTime = millis();

    // Arranque inicial
    if (!alarmaIniciada) {
        aplicarTono(BuzzerSteps[pasoActual].frequency);
        tiempoAnterior = currentTime;
        alarmaIniciada = true;
    }

    // Comprobar si ya pasó el tiempo del paso actual
    if (currentTime - tiempoAnterior >= BuzzerSteps[pasoActual].duration) {
        tiempoAnterior = currentTime;  // Reiniciar el cronómetro

        // Avanzar al siguiente paso, volviendo a 0 al terminar
        pasoActual = (pasoActual + 1) % BuzzerStepsLength;

        aplicarTono(BuzzerSteps[pasoActual].frequency);
    }
}

void stopBuzzer() {
    // Solo ejecutamos el apagado y el reinicio si la alarma estaba corriendo
    if (alarmaIniciada) {
        noTone(BUZZER_PIN);
        alarmaIniciada = false;
        pasoActual = 0;
        // tiempoAnterior no necesita reinicio porque se sobrescribe al iniciar
    }
}

// Función auxiliar para aplicar el tono o silencio
void aplicarTono(unsigned int freq) {
    if (freq > 0) {
        tone(BUZZER_PIN, freq);
    } else {
        noTone(BUZZER_PIN);
    }
}

////////////////////////////////////////////////////

SystemStatus Status = VIRGIN_EMBEDDED;

SystemEvent (*eventCaptures[])(SystemStatus systemStatus) = {
    getStockBtnEvent,
    getStockSensorsEvent,
    getSecurityBtnEvent,
    getAnomalySensorsEvent,
};

int8_t eventCaptureI = -1;
const size_t eventCapturesLength = sizeof(eventCaptures) / sizeof(eventCaptures[0]);

SystemEvent getSystemEvent() {
    eventCaptureI = (eventCaptureI + 1) % eventCapturesLength;
    return eventCaptures[eventCaptureI](Status);
}

void setup() {
    Serial.begin(57600);

    DEBUG("\r\nStarting setup...\r\n\n");

    // Stock button
    pinMode(StockBtn.pin, INPUT);
    pinMode(StockBtn.led, OUTPUT);
    digitalWrite(StockBtn.led, LOW);

    StockBtn.state = LOW;
    StockBtn.status = OFF;
    StockBtn.lastState = HIGH;
    StockBtn.lastDebounceTime = 0;

    DEBUG_BUTTON("StockBtn", StockBtn);
    DEBUG("\r\n");

    // Security button
    pinMode(SecurityBtn.pin, INPUT);
    pinMode(SecurityBtn.led, OUTPUT);
    digitalWrite(SecurityBtn.led, LOW);

    SecurityBtn.state = LOW;
    SecurityBtn.status = OFF;
    SecurityBtn.lastState = HIGH;
    SecurityBtn.lastDebounceTime = 0;

    DEBUG_BUTTON("SecurityBtn", SecurityBtn);
    DEBUG("\r\n");

    // LCD
    LCD.device->init();
    LCD.device->backlight();

    // Buzzer
    pinMode(BUZZER_PIN, OUTPUT);

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

    DEBUG("Setup completed.\r\n\n");
}

void loop() {
    switchBtnState(&StockBtn);
    switchBtnState(&SecurityBtn);

    SystemEvent event = getSystemEvent();

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
                    DEBUG_FSM(VIRGIN_EMBEDDED, event, Status);
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
                    DEBUG_FSM(STOCK_MODE, event, Status);
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
                    setBaselineWeight(&WeightSensor01);
                    setBaselineWeight(&WeightSensor02);
                    ledOff(&WeightSensor01);
                    ledOff(&WeightSensor02);
                    // TODO: Apagar el Buzzer.
                    lcdClear(&LCD);
                    Status = VIRGIN_EMBEDDED;
                    DEBUG_FSM(SECURITY_MODE, event, Status);
                    break;

                case ANOMALY_SENSOR_01:
                    ledOn(&WeightSensor01);
                    // TODO: Reproducir sonido por el Buzzer.
                    lcdPrint(&LCD, "Security alert", "on sensor #01!");
                    DEBUG_FSM(SECURITY_MODE, event, Status);
                    break;

                case ANOMALY_SENSOR_02:
                    ledOn(&WeightSensor02);
                    // TODO: Reproducir sonido por el Buzzer.
                    lcdPrint(&LCD, "Security alert", "on sensor #02!");
                    DEBUG_FSM(SECURITY_MODE, event, Status);
                    break;

                case ANOMALY_SENSORS:
                    ledOn(&WeightSensor01);
                    ledOn(&WeightSensor02);
                    // TODO: Reproducir sonido por el Buzzer.
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
