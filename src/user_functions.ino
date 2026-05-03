
#include "constants.h"
#include "structs.h"
#include "user_functions.h"

extern SystemStatus Status;

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

SystemEvent readStockBtn() {
    static int lastStatus = OFF;
    if (StockBtn.status == lastStatus) return NO_EVENT;
    lastStatus = StockBtn.status;
    return StockBtn.status == ON ? STOCK_ON : STOCK_OFF;
}

SystemEvent readStockSensors() {
    // TODO
    if (Status != STOCK_MODE) return NO_MISSING_STOCK;
    unsigned int weight01 = getWeightInGrams(&WeightSensor01);
    unsigned int weight02 = getWeightInGrams(&WeightSensor02);

    bool missing01 = weight01 < STOCK_MIN_WEIGHT;
    bool missing02 = weight02 < STOCK_MIN_WEIGHT;

    if (missing01 && missing02) return STOCK_MISSING_SENSORS;
    if (missing01) return STOCK_MISSING_SENSOR_01;
    if (missing02) return STOCK_MISSING_SENSOR_02;
    return NO_MISSING_STOCK;
}

SystemEvent readSecurityBtn() {
    static int lastStatus = OFF;
    if (SecurityBtn.status == lastStatus) return NO_EVENT;
    lastStatus = SecurityBtn.status;
    return SecurityBtn.status == ON ? SECURITY_ON : SECURITY_OFF;
}

SystemEvent readAnomalySensors() {
    // TODO
    if (Status != SECURITY_MODE) return NO_MISSING_STOCK;
    if (baselineWeight01 < 0 || baselineWeight02 < 0) return NO_MISSING_STOCK;

    int weight01 = getWeightInGrams(&WeightSensor01);
    int weight02 = getWeightInGrams(&WeightSensor02);

    bool anomaly01 = abs(weight01 - baselineWeight01) > ANOMALY_THRESHOLD;
    bool anomaly02 = abs(weight02 - baselineWeight02) > ANOMALY_THRESHOLD;

    if (anomaly01 && anomaly02) return ANOMALY_SENSORS;
    if (anomaly01) return ANOMALY_SENSOR_01;
    if (anomaly02) return ANOMALY_SENSOR_02;
    return NO_MISSING_STOCK;
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
        case NO_EVENT:
            return "NO_EVENT";
        default:
            return "UNKNOWN";
    }
}

void captureAnomalyBaseline() {
    baselineWeight01 = getWeightInGrams(&WeightSensor01);
    baselineWeight02 = getWeightInGrams(&WeightSensor02);
}

void resetAnomalyBaseline() {
    baselineWeight01 = -1;
    baselineWeight02 = -1;
}
