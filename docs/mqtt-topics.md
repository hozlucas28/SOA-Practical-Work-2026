## Topics PUBLISHED by the ESP32

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
