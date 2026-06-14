
#include <PubSubClient.h>
#include <WiFi.h>

#include "constants.h"
#include "debuggers.h"
#include "event_captures.h"
#include "mqtt.h"
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
static String TopicCmdStock;
static String TopicCmdSecurity;
static String TopicCmdAlarm;
static String TopicCmdTare;

// Persisted-tare handshake (offsets stored in Node-RED's SQLite).
static String TopicTareRequest;  // ESP32 -> Node-RED: "give me the saved offsets"
static String TopicTareState;    // Node-RED -> ESP32: the saved offsets (or null)
static String TopicTareSave;     // ESP32 -> Node-RED: "persist this offset"

static void initTopics() {
    String prefix = String("soa/") + MQTT_CLIENT_ID;

    TopicAvailability = prefix + "/availability";
    TopicStatus = prefix + "/status";
    TopicStock01 = prefix + "/shelf/01/stock";
    TopicStock02 = prefix + "/shelf/02/stock";
    TopicSecurity01 = prefix + "/shelf/01/security";
    TopicSecurity02 = prefix + "/shelf/02/security";
    TopicCmdSub = prefix + "/cmd/#";
    TopicCmdStock = prefix + "/cmd/stock";
    TopicCmdSecurity = prefix + "/cmd/security";
    TopicCmdAlarm = prefix + "/cmd/buzzer";
    TopicCmdTare = prefix + "/cmd/tare";
    TopicTareRequest = prefix + "/tare/request";
    TopicTareState = prefix + "/tare/state";
    TopicTareSave = prefix + "/tare/save";
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

// Indexed by sensor (0 -> weightSensor). Kept as arrays
// so the publish helpers take a plain `int index` instead of a `StockCache*` /
// `SecurityCache*` in their signature: the Arduino `.ino` prototype generator
// would otherwise forward-declare them before these types exist.
// status reports both mode toggles (what the user activated) plus the mode the
// FSM is actually running (`active`), since Security has priority over Stock.
struct StatusCache {
    bool isValid;
    bool stock;
    bool security;
    SystemStatus active;
};

static StatusCache StatusCacheValue = { false, false, false, UNKNOWN_SYSTEM_STATUS };
static StockCache StockCaches[2] = { { false, 0, 0, 0, false }, { false, 0, 0, 0, false } };
static SecurityCache SecurityCaches[2] = { { false, false, 0, 0 }, { false, false, 0, 0 } };

static void invalidateCache() {
    StatusCacheValue.isValid = false;
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

static void publishStatus() {
    lockButtons();
    bool stockOn = (stockBtn.status == ON);
    bool securityOn = (securityBtn.status == ON);
    unlockButtons();

    SystemStatus active = status;

    if (StatusCacheValue.isValid && StatusCacheValue.stock == stockOn && StatusCacheValue.security == securityOn &&
        StatusCacheValue.active == active) {
        return;
    }

    char payload[80];
    snprintf(
        payload,
        sizeof(payload),
        "{\"stock\":%s,\"security\":%s,\"active\":\"%s\"}",
        stockOn ? "true" : "false",
        securityOn ? "true" : "false",
        modeToStr(active)
    );
    mqttClient.publish(TopicStatus.c_str(), payload, true);

    StatusCacheValue = { true, stockOn, securityOn, active };
}

static void publishStock(WeightSensor* sensor, const String& topic, int index) {
    StockCache* cache = &StockCaches[index];

    unsigned int weight = getWeight(sensor);
    unsigned int stock = getStock(sensor);
    unsigned int minStock = sensor->minimumAcceptableStock;
    bool available = stock >= minStock;

    if (cache->valid && cache->weight == weight && cache->stock == stock && cache->min == minStock &&
        cache->available == available) {
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

    if (cache->valid && cache->secure == secure && cache->baseline == baseline && cache->current == current) {
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

// Re-publishes "availability=online" on a fixed interval so there is always
// recent outbound activity. Prevents the broker keepalive timeout when the
// Wokwi simulation clock runs slower than the broker's wall clock.
static unsigned long LastHeartbeat = 0;

static void publishHeartbeat() {
    unsigned long now = millis();
    if (LastHeartbeat != 0 && (now - LastHeartbeat) < MQTT_HEARTBEAT_MS) return;
    LastHeartbeat = now;
    mqttClient.publish(TopicAvailability.c_str(), "online", true);
}

static void publishStateIfChanged() {
    publishStatus();

    publishStock(&weightSensor, TopicStock01, 0);

    bool anomaly01 = false;
    bool anomaly02 = false;

    publishSecurity(&weightSensor, anomaly01, TopicSecurity01, 0);
}

// ---------------------------------------------------------------------------- //
//                                TARE HANDSHAKE                                //
// ---------------------------------------------------------------------------- //

// On connect the ESP32 asks Node-RED (SQLite) for the saved zero offsets. Per
// shelf: if an offset is stored, restore it (no re-tare, so product already on
// the shelf at reboot does not corrupt the zero); otherwise tare now and persist
// the result. The boot-time tare in setup() is the fallback if Node-RED is silent.
static bool TareRequested = false;
static bool TareDone = false;
static unsigned long TareRequestTime = 0;

static void publishTareSave(const char* shelf, int32_t offset) {
    char payload[48];
    snprintf(payload, sizeof(payload), "{\"shelf\":\"%s\",\"offset\":%ld}", shelf, (long)offset);
    mqttClient.publish(TopicTareSave.c_str(), payload);
}

// Minimal parser for {"01":{"offset":N,...},"02":null}. Returns true and sets
// *outOffset when the given shelf key (e.g. "\"01\"") has a stored offset.
static bool parseShelfOffset(const char* message, const char* shelfKey, int32_t* outOffset) {
    const char* p = strstr(message, shelfKey);
    if (p == nullptr) return false;
    p += strlen(shelfKey);

    const char* off = strstr(p, "\"offset\"");
    if (off == nullptr) return false;

    const char* nullp = strstr(p, "null");
    if (nullp != nullptr && nullp < off) return false;  // this shelf is null

    off = strchr(off, ':');
    if (off == nullptr) return false;

    *outOffset = (int32_t)atol(off + 1);  // atol stops at the next non-digit
    return true;
}

static void resolveShelfTare(WeightSensor* sensor, const char* message, const char* shelfKey, const char* shelf) {
    int32_t offset = 0;
    if (parseShelfOffset(message, shelfKey, &offset)) {
        setOffset(sensor, offset);
        DEBUG("Tare %s: restored saved offset %ld\r\n", shelf, (long)offset);
    } else {
        int32_t newOffset = tare(sensor);
        publishTareSave(shelf, newOffset);
        DEBUG("Tare %s: tared now, persisting offset %ld\r\n", shelf, (long)newOffset);
    }
}

static void resolveTare(const char* message) {
    resolveShelfTare(&weightSensor, message, "\"01\"", "01");
    TareDone = true;
    TareRequested = false;
}

// ---------------------------------------------------------------------------- //
//                                   COMMANDS                                   //
// ---------------------------------------------------------------------------- //

// Translates an incoming command into the same shared state a physical
// button/sensor would produce. Never touches the FSM, LCD or buzzer directly.
static void mqttCallback(char* topic, byte* payload, unsigned int length) {
    char message[160];
    unsigned int size = length < sizeof(message) - 1 ? length : sizeof(message) - 1;
    memcpy(message, payload, size);
    message[size] = '\0';

    String currentTopic(topic);

    DEBUG("MQTT cmd [%s]: %s\r\n", topic, message);

    if (currentTopic == TopicTareState) {
        resolveTare(message);
        return;
    }

    // Two independent toggles, mirroring the two physical buttons. Both modes
    // can be ON at once; the FSM already gives Security priority over Stock.
    if (currentTopic == TopicCmdStock) {
        if (strcmp(message, "ON") == 0 || strcmp(message, "OFF") == 0) {
            lockButtons();
            ButtonStatus newStatus = strcmp(message, "ON") == 0 ? ON : OFF;
            stockBtn.status = newStatus;
            digitalWrite(stockBtn.led, newStatus);
            unlockButtons();
        }
        return;
    }

    if (currentTopic == TopicCmdSecurity) {
        if (strcmp(message, "ON") == 0 || strcmp(message, "OFF") == 0) {
            lockButtons();
            ButtonStatus newStatus = strcmp(message, "ON") == 0 ? ON : OFF;
            securityBtn.status = newStatus;
            digitalWrite(securityBtn.led, newStatus);
            unlockButtons();
        }
        return;
    }

    if (currentTopic == TopicCmdAlarm) {
        if (strcmp(message, "OFF") == 0) {
            stopBuzzer(&buzzer);
        } else if (strcmp(message, "ON") == 0) {
            playBuzzer(&buzzer);
        }
        return;
    }

    if (currentTopic == TopicCmdTare) {
        if (strstr(message, "all") != nullptr) {
            setBaselineWeight(&weightSensor);
        } else if (strstr(message, "1") != nullptr) {
            setBaselineWeight(&weightSensor);
        } else if (strstr(message, "2") != nullptr) {
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
    if (lastAttempt != 0 && (now - lastAttempt) < MQTT_RECONNECTION_DELAY) return;
    lastAttempt = now;

    DEBUG("MQTT: connecting to %s:%u...\r\n", MQTT_HOST, MQTT_PORT);

    bool connected =
        mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS, TopicAvailability.c_str(), 1, true, "offline");

    if (connected) {
        DEBUG("MQTT: connected.\r\n");
        mqttClient.publish(TopicAvailability.c_str(), "online", true);
        mqttClient.subscribe(TopicCmdSub.c_str());
        mqttClient.subscribe(TopicTareState.c_str());

        // Ask Node-RED for the persisted tare offsets, once per boot.
        if (!TareDone && !TareRequested) {
            mqttClient.publish(TopicTareRequest.c_str(), "");
            TareRequested = true;
            TareRequestTime = millis();
        }

        // Republish the full current state for any app that just subscribed.
        invalidateCache();
    } else {
        DEBUG("MQTT: connection failed (state %d), will retry.\r\n", mqttClient.state());
    }
}

// ---------------------------------------------------------------------------- //
//                                     TASK                                     //
// ---------------------------------------------------------------------------- //

void xMQTTTask(void* parameters) {
    initTopics();

    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setKeepAlive(MQTT_KEEPALIVE);
    mqttClient.setCallback(mqttCallback);

    uint32_t delay = pdMS_TO_TICKS(100);

    while (true) {
        if (WiFi.status() != WL_CONNECTED) connectWifi();

        mqttReconnect();
        mqttClient.loop();

        if (mqttClient.connected()) {
            publishStateIfChanged();
            publishHeartbeat();
        }

        // Tare handshake gave no answer in time: keep the boot-time tare and stop
        // waiting for this boot (it is retried on the next reboot).
        if (TareRequested && !TareDone && (millis() - TareRequestTime) > TARE_RESPONSE_TIMEOUT_MS) {
            TareRequested = false;
            TareDone = true;
            DEBUG("Tare: no response from Node-RED, keeping boot-time tare.\r\n");
        }

        vTaskDelay(delay);
    }
}
