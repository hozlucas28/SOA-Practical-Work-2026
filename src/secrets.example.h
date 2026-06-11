#ifndef SRC_SECRETS_H_INCLUDED
#define SRC_SECRETS_H_INCLUDED

/**
 * Plantilla de credenciales. Copiar este archivo como `secrets.h` (ignorado por
 * git) y completar con los valores reales. NUNCA versionar `secrets.h`.
 *
 *   cp src/secrets.example.h src/secrets.h
 */

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

#endif  // SRC_SECRETS_H_INCLUDED
