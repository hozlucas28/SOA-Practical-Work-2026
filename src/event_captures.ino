
#include "constants.h"
#include "enums.h"
#include "event_captures.h"
#include "sync.h"
#include "user_functions.h"

SystemEvent getStockBtnEvent(SystemStatus systemStatus) {
    ButtonStatus status = OFF;

    lockButtons();
    status = stockBtn.status;
    unlockButtons();

    return status == ON ? STOCK_ON : STOCK_OFF;
}

SystemEvent getStockSensorsEvent(SystemStatus systemStatus) {
    if (systemStatus != STOCK_MODE) return NO_MISSING_STOCK;

    unsigned int stock = 0;
    unsigned int minimum = 0;
    bool valid = false;

    lockWeightSensors();

    if (weightSensor.sample.valid) {
        stock = weightSensor.sample.weight / weightSensor.product.weight;
        valid = true;
    }
    minimum = weightSensor.minimumAcceptableStock;

    unlockWeightSensors();

    bool missingStock = valid && stock < minimum;

    if (missingStock) return STOCK_MISSING_SENSOR;

    return NO_MISSING_STOCK;
}

SystemEvent getSecurityBtnEvent(SystemStatus systemStatus) {
    ButtonStatus securityStatus = OFF;
    ButtonStatus stockStatus = OFF;

    lockButtons();

    securityStatus = securityBtn.status;
    stockStatus = stockBtn.status;

    unlockButtons();

    if (securityStatus == ON) return SECURITY_ON;
    if (systemStatus == SECURITY_MODE && stockStatus == ON) return SECURITY_OFF_TO_STOCK;
    return SECURITY_OFF;
}

static bool anomaly = false;

SystemEvent getAnomalySensorsEvent(SystemStatus systemStatus) {
    if (systemStatus != SECURITY_MODE) {
        anomaly = false;
        return NO_ANOMALY;
    };

    unsigned int weight = 0;
    unsigned int baseline = 0;
    bool valid = false;

    lockWeightSensors();

    if (weightSensor.sample.valid) {
        weight = weightSensor.sample.weight;
        valid = true;
    }
    baseline = weightSensor.baselineWeight;

    unlockWeightSensors();

    if (valid) {
        anomaly = anomaly || abs((int)weight - (int)baseline) > ANOMALY_THRESHOLD;
    }

    if (anomaly) return ANOMALY_SENSOR;

    return NO_ANOMALY;
}
