#ifndef MQTT_H_INCLUDED
#define MQTT_H_INCLUDED

#include <PubSubClient.h>

#define WIFI_SSID     "Wokwi-GUEST"  // WiFi SSID of the network to which the ESP32 should connect.
#define WIFI_PASSWORD ""

#define MQTT_DEVICE_ID "corridor-01"  // Must be unique across all clients connecting to the same broker.

#define MQTT_BROKER_HOST "localhost"  // IP address of the machine running the Mosquitto broker and Node-RED server.
#define MQTT_BROKER_PORT 1883

#define MQTT_BROKER_USER     "grupo-l5"
#define MQTT_BROKER_PASSWORD "secretl5"

/**
 * @brief MQTT keepalive (in seconds).
 */
#define MQTT_KEEPALIVE 15

/**
 * @brief Interval between periodic MQTT publishes (in milliseconds).
 */
#define MQTT_PUBLISH_INTERVAL 150

extern WiFiClient espClient;
extern PubSubClient mqttClient;

/**
 * @brief Callback function for handling incoming MQTT messages. This function is called whenever a message is received
 * on a subscribed topic.
 *
 * @param topic Topic on which the message was received.
 * @param payload Payload of the message.
 * @param length Length of the payload.
 */
void mqttCallback(char* topic, byte* payload, unsigned int length);

/**
 * @brief FreeRTOS task responsible for managing MQTT communication, including connecting to the broker, subscribing to
 * topics, and publishing messages.
 */
void xMQTTTask(void* parameters);

#endif  // MQTT_H_INCLUDED
