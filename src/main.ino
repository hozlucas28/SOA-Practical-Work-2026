
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
// TODO: mejorar
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

    // TODO: Utilizar una maquina de estados.
    if (StockBtn.state == ON) {
        // TODO:
        //   - Mostrar por pantalla la cantidad de Stock en cada sensor de peso.
    }

    if (SecurityBtn.status == ON) {
        // TODO:
        //   - Mostrar por pantalla si se detecta una anomalía en un sensor de peso.
        //   - Mostrar por pantalla el/los tipo/s de anomalía/s detectada/s.
        //   - Mostrar por pantalla el/los número/s de sensor/es que registro/registraron la/s anomalía/s.
        //   - Reproducir por el Buzzer si algún sensor detecto una anomalía.
        playBuzzer();
    } else {
        stopBuzzer();
    }
}
