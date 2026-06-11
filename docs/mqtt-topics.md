<h1 align="center">MQTT Topic Contract</h1>

<p align="center">
    <strong>MQTT communication contract between the ESP32 device and the app / Node-RED</strong>
    <br>
    <strong>- SOA Practical Work [2026] · UNLaM -</strong>
</p>

## Summary

The device (ESP32 shelves with a Stock/Security FSM) integrates **bidirectionally** with the
application through a **Mosquitto** broker (with **Node-RED** as the bridge to the app). This document
is the **single source of truth** for the contract: it defines the topics, their direction,
`retain`/`QoS`, the schema of each payload, and when it is published.

The app has two screens that map directly to the **per-shelf** topics:

- **"Inventario en Tiempo Real"** (Stock mode) → one card per sensor with weight and availability
  (`shelf/NN/stock`).
- **"Seguridad Activa"** (Security mode) → one card per sensor with secure/alert state
  (`shelf/NN/security`).

> [!NOTE]
> The broker (Mosquitto), Node-RED, and the infrastructure are provided by the team outside this
> repository. This document only fixes the contract that both sides must respect.

## General conventions

- **Topic prefix:** `soa/{deviceId}/...` — `deviceId` is configurable per device (`MQTT_CLIENT_ID`,
  e.g. `gondola-01`), so several shelves can coexist without topic collisions.
- **Payloads:** UTF-8 JSON (except `availability`, which is plain text). Under 512 bytes (the limit set
  in PubSubClient with `-DMQTT_MAX_PACKET_SIZE=512`).
- **Per-sensor state:** each sensor publishes to its own topic (`shelf/01/...`, `shelf/02/...`), all
  `retain`, so a late subscriber recovers the state of **both** sensors.
- **Representation: booleans + numbers.** The firmware sends `available`/`secure` as `true`/`false`
  and the app maps them to its own text/color (DISPONIBLE/AGOTADO, SEGURO/ALERTA).
- **Product names are handled by the app** (they do not travel over MQTT).

---

## Topics PUBLISHED by the ESP32

The app / Node-RED **subscribes** to these topics.

| Topic                          | Retain | QoS | When it is published                                         | Payload    |
| ------------------------------ | :----: | :-: | ------------------------------------------------------------ | ---------- |
| `soa/{id}/availability`        |   ✅   |  1  | On connect (`online`) and as **Last Will (LWT)** (`offline`) | plain text |
| `soa/{id}/status`              |   ✅   |  0  | On every FSM transition (on-change)                          | JSON       |
| `soa/{id}/shelf/{NN}/stock`    |   ✅   |  0  | On-change of weight/stock/availability                       | JSON       |
| `soa/{id}/shelf/{NN}/security` |   ✅   |  0  | On-change of the security state                              | JSON       |

`{NN}` ∈ `01` | `02`.

### `availability`

Plain text. `online` on connect; `offline` is published automatically by the broker via the LWT if the
device disconnects ungracefully.

```
online
```

### `status`

Reports the **two mode toggles** (Stock and Security can be active at the same time) plus the mode the
FSM is actually running in `active` (Security has **priority** over Stock). `active` mapping:
`VIRGIN_EMBEDDED → IDLE`, `STOCK_MODE → STOCK`, `SECURITY_MODE → SECURITY`.

```json
{ "stock": true, "security": true, "active": "SECURITY" }
```

| Field      | Type   | Description                                                 |
| ---------- | ------ | ----------------------------------------------------------- |
| `stock`    | bool   | The user activated Stock mode (toggle / Stock button)       |
| `security` | bool   | The user activated Security mode (toggle / Security button) |
| `active`   | string | Mode running by priority: `IDLE` \| `STOCK` \| `SECURITY`   |

> Examples: `{"stock":true,"security":false,"active":"STOCK"}` (stock only) ·
> `{"stock":true,"security":true,"active":"SECURITY"}` (both active, security runs) ·
> `{"stock":false,"security":false,"active":"IDLE"}` (nothing active).

### `shelf/{NN}/stock`

Inventory state of a shelf ("Inventario en Tiempo Real" screen). Published only **on-change** (the load
cell is sampled every 200 ms; publishing every sample would flood the broker).

```json
{ "weight": 3000, "stock": 3, "min": 1, "available": true }
```

| Field       | Type | Description                           | Used in the app for            |
| ----------- | ---- | ------------------------------------- | ------------------------------ |
| `weight`    | int  | Total measured weight, in grams       | "Peso Actual: 3000 g"          |
| `stock`     | int  | Units derived (`weight / unitWeight`) | —                              |
| `min`       | int  | Minimum acceptable stock              | —                              |
| `available` | bool | `stock >= min`                        | "Estado: DISPONIBLE / AGOTADO" |

### `shelf/{NN}/security`

Security state of a shelf ("Seguridad Activa" screen). Published only **on-change**. `secure` reflects
the anomaly latch: `false` when the weight deviated from the baseline by more than the threshold (the
same thing that drives the buzzer/LED).

```json
{ "secure": true, "baseline": 3000, "current": 3000, "delta": 0 }
```

| Field      | Type | Description                                     | Used in the app for       |
| ---------- | ---- | ----------------------------------------------- | ------------------------- |
| `secure`   | bool | `true` = no anomaly, `false` = anomaly detected | "Estado: SEGURO / ALERTA" |
| `baseline` | int  | Reference weight, in grams                      | —                         |
| `current`  | int  | Current weight, in grams                        | —                         |
| `delta`    | int  | `current - baseline`, in grams                  | —                         |

---

## Tare persistence (ESP32 ⇄ Node-RED/SQLite)

Each load cell's **zero offset (tare)** is persisted in **Node-RED's SQLite** database, so a reboot
with product already on the shelf **does not corrupt the zero**. On boot, the ESP32 performs a
handshake:

| Topic                   | Direction        | Payload                                             | When                                             |
| ----------------------- | ---------------- | --------------------------------------------------- | ------------------------------------------------ |
| `soa/{id}/tare/request` | ESP32 → Node-RED | (empty)                                             | On connect (once per boot)                       |
| `soa/{id}/tare/state`   | Node-RED → ESP32 | `{"01":{"offset":N}\|null,"02":{"offset":N}\|null}` | Response of the `SELECT` against SQLite          |
| `soa/{id}/tare/save`    | ESP32 → Node-RED | `{"shelf":"01","offset":N}`                         | When the ESP32 tares a cell with no saved offset |

**ESP32 logic, per cell, on receiving `tare/state`:**

- A saved `offset` exists → `set_offset(offset)` (restores the zero, does **not** re-tare).
- It is `null` → `tare()` now → `get_offset()` → publishes `tare/save` (Node-RED does an `UPSERT`).
- If `tare/state` does not arrive within `TARE_RESPONSE_TIMEOUT_MS` → the boot-time tare stands (retried
  on the next boot).

> SQLite table: `tare(device_id, shelf, offset, updated_at, PRIMARY KEY(device_id, shelf))`.
> Node-RED also exposes tare status in `GET /api/{id}/state` (`tare.NN.done` + `ts`) for the app.

---

## Topics the ESP32 SUBSCRIBES to

The app / Node-RED **publishes** to these topics to control the device remotely. A command has the
**same effect** as operating the physical button/sensor (it is "the same action, but remote"): the
physical buttons keep working the same.

| Topic                   | QoS | Effect on the device                                   | Payload    |
| ----------------------- | :-: | ------------------------------------------------------ | ---------- |
| `soa/{id}/cmd/stock`    |  1  | Activate/deactivate Stock mode (independent toggle)    | plain text |
| `soa/{id}/cmd/security` |  1  | Activate/deactivate Security mode (independent toggle) | plain text |
| `soa/{id}/cmd/alarm`    |  1  | Mute / restore the buzzer in Security mode             | plain text |

### `cmd/stock` and `cmd/security`

Two **independent** toggles, one per mode, that mirror the two physical buttons exactly. **Both can be
`ON` at once**; the FSM gives **Security priority** (it runs Security and leaves Stock waiting; when
Security is turned off it automatically falls back to Stock if it was still active).

```
ON
```

| Topic          | Value        | Effect                       | Equivalent to   |
| -------------- | ------------ | ---------------------------- | --------------- |
| `cmd/stock`    | `ON` / `OFF` | Activate/deactivate Stock    | Stock button    |
| `cmd/security` | `ON` / `OFF` | Activate/deactivate Security | Security button |

> To "turn everything off", the app sends `cmd/stock OFF` **and** `cmd/security OFF`. The effective
> result is shown in `status.active`.

### `cmd/alarm`

```
OFF
```

| Value | Effect                                                                      |
| ----- | --------------------------------------------------------------------------- |
| `OFF` | Mutes the buzzer (the alert stays visible on LCD/LED and in `secure:false`) |
| `ON`  | Restores the buzzer                                                         |

> [!NOTE]
> By "simple mute" design: muting does not clear the alert, only the sound. The alert persists until
> leaving Security mode.

> [!NOTE]
> Re-taring the load cells is **not** a remote command: the device resolves the tare on its own (see
> [Tare persistence](#tare-persistence-esp32--node-redsqlite)).

---

## Credentials / authentication

Authentication is **at the broker-connection level**, not per topic: the device authenticates once on
connect and the broker's **ACLs** decide which topics it may publish/subscribe to.

### Broker side (Mosquitto)

- **Recommended — username + password.** In `mosquitto.conf`:
  ```conf
  allow_anonymous false
  password_file /mosquitto/config/passwd
  ```
  User created with:
  ```bash
  mosquitto_passwd -c /mosquitto/config/passwd esp32
  ```
- **Optional — ACLs.** Restrict the ESP32 user to its own prefix (`acl_file`):
  ```conf
  user esp32
  topic write soa/gondola-01/#
  topic read  soa/gondola-01/cmd/#
  ```
- For a quick credential-less demo you can use `allow_anonymous true` (not recommended).

### Firmware side (`src/secrets.h`, git-ignored)

Credentials are **never** versioned: they live in `src/secrets.h` (ignored by git) and are passed to
`mqttClient.connect()`. `src/secrets.example.h` is versioned as a template.

| Constant                 | Use                                                                 |
| ------------------------ | ------------------------------------------------------------------- |
| `WIFI_SSID`, `WIFI_PASS` | WiFi network (in Wokwi: `Wokwi-GUEST`, no password)                 |
| `MQTT_HOST`, `MQTT_PORT` | Broker endpoint (LAN IP of the Docker host, `1883`)                 |
| `MQTT_USER`, `MQTT_PASS` | Credentials passed to `mqttClient.connect()`                        |
| `MQTT_CLIENT_ID`         | Unique per-device id = the `{id}` of the topics (e.g. `gondola-01`) |

Connection with authentication + integrated Last Will (PubSubClient v2.8):

```cpp
mqttClient.connect(
    MQTT_CLIENT_ID,
    MQTT_USER, MQTT_PASS,
    "soa/gondola-01/availability", 1, true, "offline"  // LWT
);
```

---

## Flow diagram

```
                     soa/{id}/status, /shelf/+/stock, /shelf/+/security, /availability
   ESP32  ─────────────────────────────────────────────────────────────────►  Mosquitto ─► Node-RED ─► App
   (FSM)  ◄─────────────────────────────────────────────────────────────────  Mosquitto ◄─ Node-RED ◄─ App
                     soa/{id}/cmd/stock, /cmd/security, /cmd/alarm
```

---

## Quick tests

Subscribe to everything the device publishes:

```bash
mosquitto_sub -h <broker> -u esp32 -P <pass> -t 'soa/#' -v
```

Activate both modes at once (it must react the same as pressing both buttons; security runs):

```bash
mosquitto_pub -h <broker> -u esp32 -P <pass> -t 'soa/gondola-01/cmd/stock'    -m ON
mosquitto_pub -h <broker> -u esp32 -P <pass> -t 'soa/gondola-01/cmd/security' -m ON
# status -> {"stock":true,"security":true,"active":"SECURITY"}
```

Mute the buzzer:

```bash
mosquitto_pub -h <broker> -u esp32 -P <pass> -t 'soa/gondola-01/cmd/alarm' -m OFF
```
