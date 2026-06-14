
#include "constants.h"
#include "enums.h"
#include "event_captures.h"
#include "sync.h"
#include "user_functions.h"

SystemEvent getStockBtnEvent(SystemStatus systemStatus) {
    lockButtons();
    ButtonStatus status = stockBtn.status;
    unlockButtons();

    return status == ON ? STOCK_ON : STOCK_OFF;
}

SystemEvent getStockSensorEvent(SystemStatus systemStatus) {
    if (systemStatus != STOCK_MODE) return NO_MISSING_STOCK;

    bool valid = false;
    unsigned int stock = 0;

    lockWeightSensor();
    if (weightSensor.sample.isValid) {
        valid = true;
        stock = weightSensor.sample.weight / weightSensor.product.weight;
    }

    unsigned int minimum = weightSensor.minimumAcceptableStock;
    unlockWeightSensor();

    bool missingStock = valid && stock < minimum;
    if (missingStock) return STOCK_MISSING_SENSOR;

    return NO_MISSING_STOCK;
}

SystemEvent getSecurityBtnEvent(SystemStatus systemStatus) {
    lockButtons();
    ButtonStatus securityStatus = securityBtn.status;
    ButtonStatus stockStatus = stockBtn.status;
    unlockButtons();

    if (securityStatus == ON) return SECURITY_ON;
    if (systemStatus == SECURITY_MODE && stockStatus == ON) return SECURITY_OFF_TO_STOCK;
    return SECURITY_OFF;
}

bool anomaly = false;

SystemEvent getAnomalySensorEvent(SystemStatus systemStatus) {
    if (systemStatus != SECURITY_MODE) {
        anomaly = false;
        return NO_ANOMALY;
    };

    bool valid = false;
    unsigned int weight = 0;

    lockWeightSensor();
    if (weightSensor.sample.isValid) {
        valid = true;
        weight = weightSensor.sample.weight;
    }

    unsigned int baseline = weightSensor.baselineWeight;
    unlockWeightSensor();

    if (valid) {
        anomaly = anomaly || abs((int)weight - (int)baseline) > ANOMALY_THRESHOLD;
    }

    if (anomaly) return ANOMALY_SENSOR;
    return NO_ANOMALY;
}
