
#include "constants.h"
#include "structs.h"
#include "user_functions.h"

void switchBtnState(Button* btn) {
    int btnRead = digitalRead(btn->pin);
    if (btnRead != btn->lastState) btn->lastDebounceTime = millis();

    if ((millis() - btn->lastDebounceTime) > btn->debounceDelay) {
        if (btnRead != btn->state) {
            btn->state = btnRead;

            if (btn->state == HIGH) {
                int ledRead = !digitalRead(btn->led.pin);
                digitalWrite(btn->led.pin, ledRead);
                btn->status = ledRead == HIGH ? ON : OFF;
            }
        }
    }

    btn->lastState = btnRead;
}

unsigned int getWeightInGrams(WeightSensor* weightSensor) {
    if (!weightSensor->device.is_ready()) return 0;

    float currentWeight = weightSensor->device.get_units(10);

    return round(currentWeight);
}

SystemEvent readStockBtn() { return StockBtn.status == ON ? STOCK_ON : STOCK_OFF; }

SystemEvent readStockSensors() {
    // TODO
    return NO_MISSING_STOCK;
}

SystemEvent readSecurityBtn() { return SecurityBtn.status == ON ? SECURITY_ON : SECURITY_OFF; }

SystemEvent readAnomalySensors() {
    // TODO
    return SECURITY_OFF;
}

void lcdClear() { LCD.clear(); }

void lcdPrint(const String line01, const String line02) {
    LCD.setCursor(0, 0);
    LCD.print(line01);

    LCD.setCursor(0, 1);
    LCD.print(line02);
}

void ledOn(const uint8_t pin) { digitalWrite(pin, HIGH); }

void ledOff(const uint8_t pin) { digitalWrite(pin, LOW); }

const char* statusToString(SystemStatus s) {
    switch (s) {
        case VIRGIN_EMBEDDED:
            return "VIRGIN_EMBEDDED";
        case STOCK_MODE:
            return "STOCK_MODE";
        case SECURITY_MODE:
            return "SECURITY_MODE";
        default:
            return "UNKNOWN";
    }
}

const char* eventToString(SystemEvent e) {
    switch (e) {
        case STOCK_ON:
            return "STOCK_ON";
        case STOCK_OFF:
            return "STOCK_OFF";
        case STOCK_MISSING_SENSOR_01:
            return "STOCK_MISSING_SENSOR_01";
        case STOCK_MISSING_SENSOR_02:
            return "STOCK_MISSING_SENSOR_02";
        case STOCK_MISSING_SENSORS:
            return "STOCK_MISSING_SENSORS";
        case NO_MISSING_STOCK:
            return "NO_MISSING_STOCK";
        case SECURITY_ON:
            return "SECURITY_ON";
        case SECURITY_OFF:
            return "SECURITY_OFF";
        case ANOMALY_SENSOR_01:
            return "ANOMALY_SENSOR_01";
        case ANOMALY_SENSOR_02:
            return "ANOMALY_SENSOR_02";
        case ANOMALY_SENSORS:
            return "ANOMALY_SENSORS";
        default:
            return "UNKNOWN";
    }
}
