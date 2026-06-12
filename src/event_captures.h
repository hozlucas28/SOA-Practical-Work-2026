#ifndef SRC_EVENT_CAPTURES_H_INCLUDED
#define SRC_EVENT_CAPTURES_H_INCLUDED

#include "enums.h"

/**
 * Returns the `stockBtn` events.
 *
 *   - `STOCK_ON`: When `stockBtn` is latched.
 *   - `STOCK_OFF`: Otherwise.
 */
SystemEvent getStockBtnEvent(SystemStatus systemStatus);

/**
 * Returns the `weightSensor` events.
 *
 *   - `STOCK_MISSING_SENSOR`: When the calculated stock is less than the minimum acceptable one.
 *   - `NO_MISSING_STOCK`: Otherwise.
 */
SystemEvent getStockSensorEvent(SystemStatus systemStatus);

/**
 * Returns the `securityBtn` events.
 *
 *   - `SECURITY_ON`: When `securityBtn` is latched.
 *   - `SECURITY_OFF_TO_STOCK`: When the `securityBtn` is not latched, and the `stockBtn` is latched.
 *   - `SECURITY_OFF`: Otherwise.
 */
SystemEvent getSecurityBtnEvent(SystemStatus systemStatus);

/**
 * Returns the `anomalySensor` events.
 *
 *   - `ANOMALY_SENSOR`: When the `weight` is greater than or less than `weight +/- ANOMALY_THRESHOLD`.
 *   - `NO_ANOMALY`: Otherwise.
 */
SystemEvent getAnomalySensorEvent(SystemStatus systemStatus);

#endif  // SRC_EVENT_CAPTURES_H_INCLUDED
