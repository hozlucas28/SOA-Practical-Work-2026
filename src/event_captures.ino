
#include "constants.h"
#include "enums.h"
#include "event_captures.h"
#include "sync.h"
#include "user_functions.h"

SystemEvent getStockBtnEvent(SystemStatus systemStatus) {
    ButtonStatus status = OFF;

    lockButtons();
    status = StockBtn.status;
    unlockButtons();

    return status == ON ? STOCK_ON : STOCK_OFF;
}

SystemEvent getStockSensorsEvent(SystemStatus systemStatus) {
    if (systemStatus != STOCK_MODE) return NO_MISSING_STOCK;

    unsigned int stock01 = 0;
    unsigned int stock02 = 0;
    unsigned int minimum01 = 0;
    unsigned int minimum02 = 0;
    bool valid01 = false;
    bool valid02 = false;

    lockWeightSensors();

    if (WeightSensor01.sample.valid) {
        stock01 = WeightSensor01.sample.weight / WeightSensor01.product.weight;
        valid01 = true;
    }
    if (WeightSensor02.sample.valid) {
        stock02 = WeightSensor02.sample.weight / WeightSensor02.product.weight;
        valid02 = true;
    }
    minimum01 = WeightSensor01.minimumAcceptableStock;
    minimum02 = WeightSensor02.minimumAcceptableStock;

    unlockWeightSensors();

    bool missingStock01 = valid01 && stock01 < minimum01;
    bool missingStock02 = valid02 && stock02 < minimum02;

    if (missingStock01 && missingStock02) return STOCK_MISSING_SENSORS;
    if (missingStock01) return STOCK_MISSING_SENSOR_01;
    if (missingStock02) return STOCK_MISSING_SENSOR_02;

    return NO_MISSING_STOCK;
}

SystemEvent getSecurityBtnEvent(SystemStatus systemStatus) {
    ButtonStatus securityStatus = OFF;
    ButtonStatus stockStatus = OFF;

    lockButtons();

    securityStatus = SecurityBtn.status;
    stockStatus = StockBtn.status;

    unlockButtons();

    if (securityStatus == ON) return SECURITY_ON;
    if (systemStatus == SECURITY_MODE && stockStatus == ON) return SECURITY_OFF_TO_STOCK;
    return SECURITY_OFF;
}

static bool anomaly01 = false;
static bool anomaly02 = false;

SystemEvent getAnomalySensorsEvent(SystemStatus systemStatus) {
    if (systemStatus != SECURITY_MODE) {
        anomaly01 = false;
        anomaly02 = false;
        return NO_ANOMALY;
    };

    unsigned int weight01 = 0;
    unsigned int weight02 = 0;
    unsigned int baseline01 = 0;
    unsigned int baseline02 = 0;
    bool valid01 = false;
    bool valid02 = false;

    lockWeightSensors();

    if (WeightSensor01.sample.valid) {
        weight01 = WeightSensor01.sample.weight;
        valid01 = true;
    }
    if (WeightSensor02.sample.valid) {
        weight02 = WeightSensor02.sample.weight;
        valid02 = true;
    }
    baseline01 = WeightSensor01.baselineWeight;
    baseline02 = WeightSensor02.baselineWeight;

    unlockWeightSensors();

    if (valid01) {
        anomaly01 = anomaly01 || abs((int)weight01 - (int)baseline01) > ANOMALY_THRESHOLD;
    }
    if (valid02) {
        anomaly02 = anomaly02 || abs((int)weight02 - (int)baseline02) > ANOMALY_THRESHOLD;
    }

    if (anomaly01 && anomaly02) return ANOMALY_SENSORS;
    if (anomaly01) return ANOMALY_SENSOR_01;
    if (anomaly02) return ANOMALY_SENSOR_02;

    return NO_ANOMALY;
}

void getAnomalyLatch(bool* anomaly01Out, bool* anomaly02Out) {
    *anomaly01Out = anomaly01;
    *anomaly02Out = anomaly02;
}
