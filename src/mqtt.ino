
#include <PubSubClient.h>
#include <WiFi.h>

#include "constants.h"
#include "debuggers.h"
#include "event_captures.h"
#include "mqtt.h"
#include "secrets.h"
#include "sync.h"
#include "user_functions.h"

// ---------------------------------------------------------------------------- //
//                                    CLIENT                                    //
// ---------------------------------------------------------------------------- //

static WiFiClient espClient;
static PubSubClient mqttClient(espClient);

// ---------------------------------------------------------------------------- //
//                                    TOPICS                                    //
// ---------------------------------------------------------------------------- //

// Built once in `initTopics()` from the `soa/<MQTT_CLIENT_ID>/...` prefix.
static String TopicAvailability;
static String TopicStatus;
static String TopicStock01;
static String TopicStock02;
static String TopicSecurity01;
static String TopicSecurity02;
static String TopicCmdSub;  // wildcard subscription: soa/<id>/cmd/#
static String TopicCmdMode;
static String TopicCmdAlarm;
static String TopicCmdTare;

static void initTopics() {
    String prefix = String("soa/") + MQTT_CLIENT_ID;

    TopicAvailability = prefix + "/availability";
    TopicStatus = prefix + "/status";
    TopicStock01 = prefix + "/shelf/01/stock";
    TopicStock02 = prefix + "/shelf/02/stock";
    TopicSecurity01 = prefix + "/shelf/01/security";
    TopicSecurity02 = prefix + "/shelf/02/security";
    TopicCmdSub = prefix + "/cmd/#";
    TopicCmdMode = prefix + "/cmd/mode";
    TopicCmdAlarm = prefix + "/cmd/alarm";
    TopicCmdTare = prefix + "/cmd/tare";
}

// ---------------------------------------------------------------------------- //
//                              PUBLISH STATE CACHE                             //
// ---------------------------------------------------------------------------- //

// Last published values, so the task only publishes what actually changed.
// `valid == false` forces a (re)publish, used on first run and after a
// reconnection so a freshly subscribed app gets the full current state.

struct StockCache {
    bool valid;
    unsigned int weight;
    unsigned int stock;
    unsigned int min;
    bool available;
};

struct SecurityCache {
    bool valid;
    bool secure;
    unsigned int baseline;
    unsigned int current;
};

// Indexed by sensor (0 -> WeightSensor01, 1 -> WeightSensor02). Kept as arrays
// so the publish helpers take a plain `int index` instead of a `StockCache*` /
// `SecurityCache*` in their signature: the Arduino `.ino` prototype generator
// would otherwise forward-declare them before these types exist.
static bool ModeCacheValid = false;
static SystemStatus ModeCache = UNKNOWN_SYSTEM_STATUS;
static StockCache StockCaches[2] = { { false, 0, 0, 0, false }, { false, 0, 0, 0, false } };
static SecurityCache SecurityCaches[2] = { { false, false, 0, 0 }, { false, false, 0, 0 } };

static void invalidateCache() {
    ModeCacheValid = false;
    StockCaches[0].valid = false;
    StockCaches[1].valid = false;
    SecurityCaches[0].valid = false;
    SecurityCaches[1].valid = false;
}

// ---------------------------------------------------------------------------- //
//                                   PUBLISH                                    //
// ---------------------------------------------------------------------------- //

static const char* modeToStr(SystemStatus status) {
    switch (status) {
        case STOCK_MODE:
            return "STOCK";

        case SECURITY_MODE:
            return "SECURITY";

        default:
            return "IDLE";
    }
}

static void publishMode() {
    SystemStatus mode = Status;
    if (ModeCacheValid && mode == ModeCache) return;

    char payload[24];
    snprintf(payload, sizeof(payload), "{\"mode\":\"%s\"}", modeToStr(mode));
    mqttClient.publish(TopicStatus.c_str(), payload, true);

    ModeCache = mode;
    ModeCacheValid = true;
}

static void publishStock(WeightSensor* sensor, const String& topic, int index) {
    StockCache* cache = &StockCaches[index];

    unsigned int weight = getWeight(sensor);
    unsigned int stock = getStock(sensor);
    unsigned int minStock = sensor->minimumAcceptableStock;
    bool available = stock >= minStock;

    if (cache->valid && cache->weight == weight && cache->stock == stock &&
        cache->min == minStock && cache->available == available) {
        return;
    }

    char payload[96];
    snprintf(
        payload,
        sizeof(payload),
        "{\"weight\":%u,\"stock\":%u,\"min\":%u,\"available\":%s}",
        weight,
        stock,
        minStock,
        available ? "true" : "false"
    );
    mqttClient.publish(topic.c_str(), payload, true);

    *cache = { true, weight, stock, minStock, available };
}

static void publishSecurity(WeightSensor* sensor, bool anomaly, const String& topic, int index) {
    SecurityCache* cache = &SecurityCaches[index];

    bool secure = !anomaly;
    unsigned int baseline = sensor->baselineWeight;
    unsigned int current = getWeight(sensor);

    if (cache->valid && cache->secure == secure && cache->baseline == baseline &&
        cache->current == current) {
        return;
    }

    char payload[96];
    snprintf(
        payload,
        sizeof(payload),
        "{\"secure\":%s,\"baseline\":%u,\"current\":%u,\"delta\":%d}",
        secure ? "true" : "false",
        baseline,
        current,
        (int)current - (int)baseline
    );
    mqttClient.publish(topic.c_str(), payload, true);

    *cache = { true, secure, baseline, current };
}

static void publishStateIfChanged() {
    publishMode();

    publishStock(&WeightSensor01, TopicStock01, 0);
    publishStock(&WeightSensor02, TopicStock02, 1);

    bool anomaly01 = false;
    bool anomaly02 = false;
    getAnomalyLatch(&anomaly01, &anomaly02);

    publishSecurity(&WeightSensor01, anomaly01, TopicSecurity01, 0);
    publishSecurity(&WeightSensor02, anomaly02, TopicSecurity02, 1);
}

// ---------------------------------------------------------------------------- //
//                                   COMMANDS                                   //
// ---------------------------------------------------------------------------- //

// Translates an incoming command into the same shared state a physical
// button/sensor would produce. Never touches the FSM, LCD or buzzer directly.
static void mqttCallback(char* topic, byte* payload, unsigned int length) {
    char message[64];
    unsigned int size = length < sizeof(message) - 1 ? length : sizeof(message) - 1;
    memcpy(message, payload, size);
    message[size] = '\0';

    String currentTopic(topic);

    DEBUG("MQTT cmd [%s]: %s\r\n", topic, message);

    if (currentTopic == TopicCmdMode) {
        lockButtons();

        if (strcmp(message, "STOCK") == 0) {
            applyButtonStatus(&StockBtn, ON);
            applyButtonStatus(&SecurityBtn, OFF);
        } else if (strcmp(message, "SECURITY") == 0) {
            applyButtonStatus(&SecurityBtn, ON);
        } else if (strcmp(message, "OFF") == 0) {
            applyButtonStatus(&StockBtn, OFF);
            applyButtonStatus(&SecurityBtn, OFF);
        }

        unlockButtons();
        return;
    }

    if (currentTopic == TopicCmdAlarm) {
        if (strcmp(message, "OFF") == 0) {
            setAlarmMuted(true);
            stopBuzzer(&Alarm);
        } else if (strcmp(message, "ON") == 0) {
            setAlarmMuted(false);
        }
        return;
    }

    if (currentTopic == TopicCmdTare) {
        if (strstr(message, "all") != nullptr) {
            setBaselineWeight(&WeightSensor01);
            setBaselineWeight(&WeightSensor02);
        } else if (strstr(message, "1") != nullptr) {
            setBaselineWeight(&WeightSensor01);
        } else if (strstr(message, "2") != nullptr) {
            setBaselineWeight(&WeightSensor02);
        }
        return;
    }
}

// ---------------------------------------------------------------------------- //
//                                  CONNECTION                                  //
// ---------------------------------------------------------------------------- //

static void connectWifi() {
    if (WiFi.status() == WL_CONNECTED) return;

    DEBUG("WiFi: connecting to \"%s\"...\r\n", WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < 20000) {
        vTaskDelay(pdMS_TO_TICKS(250));
    }

    if (WiFi.status() == WL_CONNECTED) {
        DEBUG("WiFi: connected (%s).\r\n", WiFi.localIP().toString().c_str());
    } else {
        DEBUG("WiFi: connection timed out, will retry.\r\n");
    }
}

static void mqttReconnect() {
    if (mqttClient.connected()) return;

    static unsigned long lastAttempt = 0;
    unsigned long now = millis();
    if (lastAttempt != 0 && (now - lastAttempt) < MQTT_RECONNECT_INTERVAL_MS) return;
    lastAttempt = now;

    DEBUG("MQTT: connecting to %s:%u...\r\n", MQTT_HOST, MQTT_PORT);

    bool connected = mqttClient.connect(
        MQTT_CLIENT_ID,
        MQTT_USER,
        MQTT_PASS,
        TopicAvailability.c_str(),
        1,
        true,
        "offline"
    );

    if (connected) {
        DEBUG("MQTT: connected.\r\n");
        mqttClient.publish(TopicAvailability.c_str(), "online", true);
        mqttClient.subscribe(TopicCmdSub.c_str());

        // Republish the full current state for any app that just subscribed.
        invalidateCache();
    } else {
        DEBUG("MQTT: connection failed (state %d), will retry.\r\n", mqttClient.state());
    }
}

// ---------------------------------------------------------------------------- //
//                                     TASK                                     //
// ---------------------------------------------------------------------------- //

void xMqttTask(void* parameters) {
    initTopics();

    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

    while (true) {
        if (WiFi.status() != WL_CONNECTED) connectWifi();

        mqttReconnect();
        mqttClient.loop();

        if (mqttClient.connected()) publishStateIfChanged();

        vTaskDelay(pdMS_TO_TICKS(MQTT_TASK_PERIOD_MS));
    }
}
