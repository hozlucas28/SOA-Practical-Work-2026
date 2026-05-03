
#include "constants.h"
#include "enums.h"
#include "event_captures.h"

SystemEvent getStockBtnEvent() {
    if (StockBtn.status == ON) return STOCK_ON;
    return STOCK_OFF;
}

SystemEvent getStockSensorsEvent() {
    // TODO
    return UNKNOWN_SYSTEM_EVENT;
}

SystemEvent getSecurityBtnEvent() {
    if (SecurityBtn.status == ON) return SECURITY_ON;
    return SECURITY_OFF;
}

SystemEvent getAnomalySensorsEvent() {
    // TODO
    return UNKNOWN_SYSTEM_EVENT;
}
