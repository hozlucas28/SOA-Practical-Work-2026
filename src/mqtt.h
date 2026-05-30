#ifndef SRC_MQTT_H_INCLUDED
#define SRC_MQTT_H_INCLUDED

/**
 * Owns the WiFi + MQTT connection. The only task that touches the network, so
 * the FSM never blocks on it. Connects to the broker (with a Last Will of
 * `availability=offline`), subscribes to the `cmd/#` topics and translates each
 * command into the same shared state a physical button/sensor would produce;
 * and publishes the device state (mode, per-shelf stock and security) on-change.
 * Param: unused (NULL). Period: `MQTT_TASK_PERIOD_MS`.
 */
void xMqttTask(void* parameters);

#endif  // SRC_MQTT_H_INCLUDED
