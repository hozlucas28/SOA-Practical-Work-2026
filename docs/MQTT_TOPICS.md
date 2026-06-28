# MQTT Topics Reference

## Overview

This document describes all MQTT topics used by the SOA Practical Work 2026 project, including the ESP32 firmware, Node-RED, and the Android app for stock gondola management and security.

**MQTT Broker Configuration:**

- Host: `127.0.0.1` (localhost) or Docker service name `mosquitto`
- Port: `1883` (MQTT), `9001` (WebSockets)
- User: `grupo-l5`
- Password: `secretl5`

---

## Topic Hierarchy

### Root Topic

```
soa/gondola-01/
```

- **Client ID:** `gondola-01` (configurable in `src/mqtt.h`)
- **Purpose:** Namespace for all messages related to Gondola Unit #01

---

## Command Topics (Device → Broker)

### Mode Command

```
soa/gondola-01/cmd/mode
```

- **Direction:** Subscribed by ESP32
- **Payload:** `STOCK_MODE`, `SECURITY_MODE`, `MAINTENANCE_MODE`
- **Example:**
  ```bash
  mosquitto_pub -h 127.0.0.1 -u grupo-l5 -P secretl5 \
    -t "soa/gondola-01/cmd/mode" -m "STOCK_MODE"
  ```
- **Use Case:** Switch operational mode of the gondola

### Manual Lock Command

```
soa/gondola-01/cmd/lock
```

- **Direction:** Subscribed by ESP32
- **Payload:** `LOCK`, `UNLOCK`
- **Example:**
  ```bash
  mosquitto_pub -h 127.0.0.1 -u grupo-l5 -P secretl5 \
    -t "soa/gondola-01/cmd/lock" -m "LOCK"
  ```
- **Use Case:** Manually lock/unlock the security mechanism

### Calibration Command

```
soa/gondola-01/cmd/calibrate
```

- **Direction:** Subscribed by ESP32
- **Payload:** `START`, `CANCEL`
- **Example:**
  ```bash
  mosquitto_pub -h 127.0.0.1 -u grupo-l5 -P secretl5 \
    -t "soa/gondola-01/cmd/calibrate" -m "START"
  ```
- **Use Case:** Trigger sensor calibration sequence

---

## Status Topics (Device → Cloud/App)

### Stock Level Status

```
soa/gondola-01/stock/level
```

- **Direction:** Published by ESP32
- **Payload:** Integer (0-100) representing stock percentage
- **Frequency:** Published when level changes or periodically
- **Example Response:**
  ```
  Topic: soa/gondola-01/stock/level
  Payload: 75
  ```
- **Subscribe to monitor:**
  ```bash
  mosquitto_sub -h 127.0.0.1 -u grupo-l5 -P secretl5 \
    -t "soa/gondola-01/stock/level" -v
  ```

### Security Status

```
soa/gondola-01/security/status
```

- **Direction:** Published by ESP32
- **Payload:** `LOCKED`, `UNLOCKED`, `ALARM`
- **Frequency:** Published when status changes
- **Example Response:**
  ```
  Topic: soa/gondola-01/security/status
  Payload: LOCKED
  ```

### Security Alert

```
soa/gondola-01/security/alert
```

- **Direction:** Published by ESP32
- **Payload:** JSON with alert details
- **Example Payload:**
  ```json
  {
    "type": "UNAUTHORIZED_ACCESS",
    "timestamp": 1704067200,
    "severity": "HIGH"
  }
  ```
- **Subscribe to alerts:**
  ```bash
  mosquitto_sub -h 127.0.0.1 -u grupo-l5 -P secretl5 \
    -t "soa/gondola-01/security/alert" -v
  ```

### Operating Mode

```
soa/gondola-01/mode
```

- **Direction:** Published by ESP32
- **Payload:** `STOCK_MODE`, `SECURITY_MODE`, `MAINTENANCE_MODE`, `ERROR`
- **Frequency:** Published when mode changes
- **Example Response:**
  ```
  Topic: soa/gondola-01/mode
  Payload: STOCK_MODE
  ```

### System Health

```
soa/gondola-01/health/status
```

- **Direction:** Published by ESP32
- **Payload:** `OK`, `WARNING`, `ERROR`
- **Frequency:** Published periodically (e.g., every 30 seconds)

### Component Status

```
soa/gondola-01/health/component/<name>
```

- **Direction:** Published by ESP32
- **Payload:** `OK`, `WARNING`, `ERROR`
- **Component Names:**
  - `display` - LCD/OLED display
  - `buzzer` - Audible alarm
  - `rfid` - RFID reader
  - `temperature` - Temperature sensor
  - `humidity` - Humidity sensor
  - `lock` - Electronic lock mechanism
  - `wifi` - WiFi/MQTT connectivity
- **Example:**
  ```bash
  mosquitto_sub -h 127.0.0.1 -u grupo-l5 -P secretl5 \
    -t "soa/gondola-01/health/component/+\" -v
  ```

---

`cloudflare`

## Reference: Complete Topic List

| Topic                               | Direction   | Payload         | Purpose                     |
| ----------------------------------- | ----------- | --------------- | --------------------------- |
| `soa/gondola-01/cmd/mode`           | ← Subscribe | String (enum)   | Set operational mode        |
| `soa/gondola-01/cmd/lock`           | ← Subscribe | String (enum)   | Lock/unlock mechanism       |
| `soa/gondola-01/cmd/calibrate`      | ← Subscribe | String (enum)   | Start/cancel calibration    |
| `soa/gondola-01/mode`               | → Publish   | String (enum)   | Current operating mode      |
| `soa/gondola-01/stock/level`        | → Publish   | Integer (0-100) | Stock percentage            |
| `soa/gondola-01/security/status`    | → Publish   | String (enum)   | Lock status                 |
| `soa/gondola-01/security/alert`     | → Publish   | JSON            | Security alert details      |
| `soa/gondola-01/health/status`      | → Publish   | String (enum)   | Overall system health       |
| `soa/gondola-01/health/component/*` | → Publish   | String (enum)   | Individual component status |

## Credentials

`grupo-l5`
`secretl5`
