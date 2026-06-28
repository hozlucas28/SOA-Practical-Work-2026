#ifndef EVENT_CAPTURES_H_INCLUDED
#define EVENT_CAPTURES_H_INCLUDED

#include "enums.h"

/**
 * @brief Returns the `stockBtn` events.
 *
 * @return
 *   - `STOCK_ON`: When `stockBtn` is latched.
 *
 *   - `STOCK_OFF`: Otherwise.
 */
const SystemEvent getStockBtnEvent(const SystemStatus status);

/**
 * @brief Returns the `weightSensor`'s events.
 *
 * @return
 *   - `STOCK_MISSING_SENSOR_01`: When the calculated stock is less than the minimum acceptable one.
 *
 *   - `NO_MISSING_STOCK`: Otherwise.
 */
const SystemEvent getStockSensorEvent(const SystemStatus status);

/**
 * @brief Returns the `securityBtn` events.
 *
 * @return
 *   - `SECURITY_ON`: When `securityBtn` is latched.
 *
 *   - `SECURITY_OFF_TO_STOCK`: When the `securityBtn` is not latched, and the `stockBtn` is latched.
 *
 *   - `SECURITY_OFF`: Otherwise.
 */
const SystemEvent getSecurityBtnEvent(const SystemStatus status);

/**
 * @brief Returns the `anomalySensor` events.
 *
 * @return
 *   - `ANOMALY_SENSOR_01`: When the `weight` is greater than or less than `weight +/- ANOMALY_THRESHOLD`.
 *
 *   - `NO_ANOMALY`: Otherwise.
 */
const SystemEvent getAnomalySensorEvent(const SystemStatus status);

#endif  // EVENT_CAPTURES_H_INCLUDED
