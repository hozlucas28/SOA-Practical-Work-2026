
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

    unsigned int stock01 = getStock(&WeightSensor01);
    unsigned int stock02 = getStock(&WeightSensor02);

    bool missingStock01 = stock01 < WeightSensor01.minimumAcceptableStock;
    bool missingStock02 = stock02 < WeightSensor02.minimumAcceptableStock;

    if (missingStock01 && missingStock02) return STOCK_MISSING_SENSORS;
    if (missingStock01) return STOCK_MISSING_SENSOR_01;
    if (missingStock02) return STOCK_MISSING_SENSOR_02;

    return NO_MISSING_STOCK;
}

SystemEvent getSecurityBtnEvent(SystemStatus systemStatus) {
    if (SecurityBtn.status == ON) return SECURITY_ON;
    return SECURITY_OFF;
}

bool anomaly01 = false;
bool anomaly02 = false;

SystemEvent getAnomalySensorsEvent(SystemStatus systemStatus) {
    if (systemStatus != SECURITY_MODE) {
        anomaly01 = false;
        anomaly02 = false;
        return NO_ANOMALY;
    };

    unsigned int weight01 = getWeight(&WeightSensor01);
    unsigned int weight02 = getWeight(&WeightSensor02);

    anomaly01 = anomaly01 || abs((int)weight01 - (int)WeightSensor01.baselineWeight) > ANOMALY_THRESHOLD;
    anomaly02 = anomaly02 || abs((int)weight02 - (int)WeightSensor02.baselineWeight) > ANOMALY_THRESHOLD;

    if (anomaly01 && anomaly02) return ANOMALY_SENSORS;
    if (anomaly01) return ANOMALY_SENSOR_01;
    if (anomaly02) return ANOMALY_SENSOR_02;

    return NO_ANOMALY;
}
