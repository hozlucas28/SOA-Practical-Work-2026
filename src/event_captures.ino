
#include "constants.h"
#include "enums.h"
#include "event_captures.h"
#include "user_functions.h"

SystemEvent getStockBtnEvent(SystemStatus systemStatus) {
    if (StockBtn.status == ON) return STOCK_ON;
    return STOCK_OFF;
}

SystemEvent getStockSensorsEvent(SystemStatus systemStatus) {
    if (systemStatus != STOCK_MODE) return NO_MISSING_STOCK;

    unsigned int stock01 = 0;
    unsigned int stock02 = 0;
    bool valid01 = tryGetStock(&WeightSensor01, &stock01);
    bool valid02 = tryGetStock(&WeightSensor02, &stock02);

    bool missingStock01 = valid01 && stock01 < WeightSensor01.minimumAcceptableStock;
    bool missingStock02 = valid02 && stock02 < WeightSensor02.minimumAcceptableStock;

    if (missingStock01 && missingStock02) return STOCK_MISSING_SENSORS;
    if (missingStock01) return STOCK_MISSING_SENSOR_01;
    if (missingStock02) return STOCK_MISSING_SENSOR_02;

    return NO_MISSING_STOCK;
}

SystemEvent getSecurityBtnEvent(SystemStatus systemStatus) {
    if (SecurityBtn.status == ON) return SECURITY_ON;
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
    bool valid01 = tryGetWeight(&WeightSensor01, &weight01);
    bool valid02 = tryGetWeight(&WeightSensor02, &weight02);

    if (valid01) {
        anomaly01 = anomaly01 || abs((int)weight01 - (int)WeightSensor01.baselineWeight) > ANOMALY_THRESHOLD;
    }
    if (valid02) {
        anomaly02 = anomaly02 || abs((int)weight02 - (int)WeightSensor02.baselineWeight) > ANOMALY_THRESHOLD;
    }

    if (anomaly01 && anomaly02) return ANOMALY_SENSORS;
    if (anomaly01) return ANOMALY_SENSOR_01;
    if (anomaly02) return ANOMALY_SENSOR_02;

    return NO_ANOMALY;
}
