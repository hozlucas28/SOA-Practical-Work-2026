
#include <Arduino.h>

// Defines (ESP32 pin's designations, structs, etc.)
#include "enums.h"
#include "pins.h"
#include "structs.h"

// Constants and variables (structs, variables, etc.)
#include "constants.h"

// User functions (converters, utilities, etc.)
#include "user_functions.h"

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

SystemEvent (*eventCaptures[])() = {
    readStockBtn,
    readStockSensors,
    readSecurityBtn,
    readAnomalySensors,
};

uint8_t eventCaptureI = -1;
const size_t eventCapturesLength = sizeof(eventCaptures) / sizeof(eventCaptures[0]);

SystemEvent getSystemEvent() {
    eventCaptureI = (eventCaptureI + 1) % eventCapturesLength;
    return eventCaptures[eventCaptureI]();
}

void setup() {
    Serial.begin(57600);

    // Stock button
    pinMode(StockBtn.pin, INPUT);
    pinMode(StockBtn.led.pin, OUTPUT);
    digitalWrite(StockBtn.led.pin, LOW);

    StockBtn.state = LOW;
    StockBtn.status = OFF;
    StockBtn.lastState = HIGH;
    StockBtn.lastDebounceTime = 0;

    // Security button
    pinMode(SecurityBtn.pin, INPUT);
    pinMode(SecurityBtn.led.pin, OUTPUT);
    digitalWrite(SecurityBtn.led.pin, LOW);

    SecurityBtn.state = LOW;
    SecurityBtn.status = OFF;
    SecurityBtn.lastState = HIGH;
    SecurityBtn.lastDebounceTime = 0;

    // LCD
    LCD.init();
    LCD.backlight();

    // Buzzer
    pinMode(BUZZER_PIN, OUTPUT);

    // Weight sensors
    WeightSensor01.device.begin(WeightSensor01.dtPin, WeightSensor01.sckPin);
    pinMode(WeightSensor01.led.pin, OUTPUT);

    WeightSensor02.device.begin(WeightSensor02.dtPin, WeightSensor02.sckPin);
    pinMode(WeightSensor02.led.pin, OUTPUT);
}

void loop() {
    switchBtnState(&StockBtn);
    switchBtnState(&SecurityBtn);

    SystemEvent event = getSystemEvent();

    // TODO:
    // - Comprobar el correcto funcionamiento de la FSM en código.
    // - Actualizar el diagrama FSM para que coincida con este código.
    // (https://drive.google.com/drive/folders/1sVbpg8k7hKJE2epQFJ-_lZe8hzr9lcrr?usp=drive_link)
    // - Pasar el diagrama FSM a un gráfico de nodos.
    switch (Status) {
        case VIRGIN_EMBEDDED:
            switch (event) {
                case STOCK_ON:
                    Status = STOCK_MODE;
                    break;

                case SECURITY_ON:
                    lcdPrint("Security mode", "");
                    Status = SECURITY_MODE;
                    break;

                default:
                    lcdPrint("SOA - Team L5", "Stock control");
                    break;
            }
            break;

        case STOCK_MODE:
            switch (event) {
                case STOCK_OFF:
                    ledOff(WeightSensor01.led.pin);
                    ledOff(WeightSensor02.led.pin);
                    lcdClear();
                    Status = VIRGIN_EMBEDDED;
                    break;

                case STOCK_MISSING_SENSOR_01:
                    ledOn(WeightSensor01.led.pin);
                    ledOff(WeightSensor02.led.pin);
                    lcdPrint("Stock missing", "on sensor #01!");
                    break;

                case STOCK_MISSING_SENSOR_02:
                    ledOff(WeightSensor01.led.pin);
                    ledOn(WeightSensor02.led.pin);
                    lcdPrint("Stock missing", "on sensor #02!");
                    break;

                case STOCK_MISSING_SENSORS:
                    ledOn(WeightSensor01.led.pin);
                    ledOn(WeightSensor02.led.pin);
                    lcdPrint("Stock missing", "on all sensors!");
                    break;

                case NO_MISSING_STOCK:
                    ledOff(WeightSensor01.led.pin);
                    ledOff(WeightSensor02.led.pin);
                    lcdPrint("Stock #01 = XXX", "Stock #02 = YYY");  // TODO: Mostrar el Stock medido por cada sensor.
                    break;

                case SECURITY_ON:
                    ledOff(WeightSensor01.led.pin);
                    ledOff(WeightSensor02.led.pin);
                    lcdClear();
                    lcdPrint("Security mode", "");
                    Status = SECURITY_MODE;
                    break;
            }
            break;

        case SECURITY_MODE:
            switch (event) {
                case SECURITY_OFF:
                    ledOff(WeightSensor01.led.pin);
                    ledOff(WeightSensor02.led.pin);
                    // TODO: Apagar el Buzzer.
                    lcdClear();
                    Status = VIRGIN_EMBEDDED;
                    break;

                case ANOMALY_SENSOR_01:
                    ledOn(WeightSensor01.led.pin);
                    // TODO: Reproducir sonido por el Buzzer.
                    lcdPrint("Security alert", "on sensor #01!");
                    break;

                case ANOMALY_SENSOR_02:
                    ledOn(WeightSensor02.led.pin);
                    // TODO: Reproducir sonido por el Buzzer.
                    lcdPrint("Security alert", "on sensor #02!");
                    break;

                case ANOMALY_SENSORS:
                    ledOn(WeightSensor01.led.pin);
                    ledOn(WeightSensor02.led.pin);
                    // TODO: Reproducir sonido por el Buzzer.
                    lcdPrint("Security alert", "on all sensors!");
                    break;
            }
            break;
    }
}
