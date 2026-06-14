#ifndef MQTT_H_INCLUDED
#define MQTT_H_INCLUDED

// ---------------------------------------------------------------------------- //
//                                     WIFI                                     //
// ---------------------------------------------------------------------------- //

/** SSID de la red WiFi. En Wokwi usar "Wokwi-GUEST". */
#define WIFI_SSID "Wokwi-GUEST"

/** Contraseña de la red WiFi. En Wokwi dejar vacío. */
#define WIFI_PASS ""

// ---------------------------------------------------------------------------- //
//                                     MQTT                                     //
// ---------------------------------------------------------------------------- //

/** Host del broker Mosquitto (IP LAN del host del Docker). */
#define MQTT_HOST "192.168.0.100"

/** Puerto del broker MQTT. */
#define MQTT_PORT 1883

/** Usuario MQTT (creado con `mosquitto_passwd`). */
#define MQTT_USER "esp32"

/** Contraseña MQTT. */
#define MQTT_PASS "changeme"

/** Identificador único del dispositivo. Se usa como clientId y como `{id}` de los topics. */
#define MQTT_CLIENT_ID "gondola-01"

/**
 * Owns the WiFi + MQTT connection. The only task that touches the network, so
 * the FSM never blocks on it. Connects to the broker (with a Last Will of
 * `availability=offline`), subscribes to the `cmd/#` topics and translates each
 * command into the same shared state a physical button/sensor would produce;
 * and publishes the device state (mode, per-shelf stock and security) on-change.
 * Param: unused (NULL).
 */
void xMQTTTask(void* parameters);

#endif  // MQTT_H_INCLUDED
