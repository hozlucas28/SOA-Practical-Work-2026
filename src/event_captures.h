#ifndef SRC_EVENT_CAPTURES_H_INCLUDED
#define SRC_EVENT_CAPTURES_H_INCLUDED

#include "enums.h"

/** Maps the cached `stockBtn.status` to `STOCK_ON` / `STOCK_OFF`. */
SystemEvent getStockBtnEvent(SystemStatus systemStatus);

/**
 * Compares cached stock counts against the per-sensor minimums and returns
 * the matching `STOCK_MISSING_*` / `NO_MISSING_STOCK` event. Returns
 * `NO_MISSING_STOCK` when the FSM is not in `STOCK_MODE` so the dispatcher
 * does nothing in other modes.
 */
SystemEvent getStockSensorsEvent(SystemStatus systemStatus);

/** Maps the cached `securityBtn.status` to `SECURITY_*` events. */
SystemEvent getSecurityBtnEvent(SystemStatus systemStatus);

/**
 * Latches a per-sensor anomaly when the cached weight diverges from the
 * baseline by more than `ANOMALY_THRESHOLD`. Skips the comparison when the
 * cached sample is invalid (HX711 unready) so a transient unready does not
 * trigger a false alarm. Latches reset whenever the FSM is not in
 * `SECURITY_MODE`.
 */
SystemEvent getAnomalySensorsEvent(SystemStatus systemStatus);

#endif  // SRC_EVENT_CAPTURES_H_INCLUDED
