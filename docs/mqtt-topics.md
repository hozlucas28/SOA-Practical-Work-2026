<h1 align="center">MQTT Topic Contract</h1>

<p align="center">
    <strong>Contrato de comunicación MQTT entre el embebido ESP32 y la app / Node-RED</strong>
    <br>
    <strong>- SOA Practical Work [2026] · UNLaM -</strong>
</p>

## Resumen

El embebido (góndolas ESP32 con FSM Stock/Security) se integra de forma **bidireccional** con la
aplicación a través de un broker **Mosquitto** (con **Node-RED** como puente hacia la app). Este
documento es la **fuente única de verdad** del contrato: define los topics, su dirección,
`retain`/`QoS`, el esquema de cada payload y cuándo se publica.

La app tiene dos pantallas que mapean directo a los topics **por sensor**:

- **"Inventario en Tiempo Real"** (modo Stock) → una tarjeta por sensor con peso y disponibilidad
  (`shelf/NN/stock`).
- **"Seguridad Activa"** (modo Security) → una tarjeta por sensor con estado seguro/alerta
  (`shelf/NN/security`).

> [!NOTE]
> El broker (Mosquitto), Node-RED y la infraestructura los provee el equipo por fuera de este
> repositorio. Este documento solo fija el contrato que ambos lados deben respetar.

## Convención general

- **Prefijo de topic:** `soa/{deviceId}/...` — `deviceId` es configurable por dispositivo
  (`MQTT_CLIENT_ID`, ej. `gondola-01`), para soportar varias góndolas sin colisión de topics.
- **Payloads:** JSON UTF-8 (salvo `availability`, texto plano). Bajo 512 bytes (límite configurado en
  PubSubClient con `-DMQTT_MAX_PACKET_SIZE=512`).
- **Estado por sensor:** cada sensor publica en su propio topic (`shelf/01/...`, `shelf/02/...`), todos
  `retain`, así un suscriptor que se conecta tarde recupera el estado de **ambos** sensores.
- **Representación: booleans + números.** El firmware manda `available`/`secure` como `true`/`false`
  y la app los mapea a su propio texto/color (DISPONIBLE/AGOTADO, SEGURO/ALERTA).
- **Los nombres de producto los maneja la app** (no viajan por MQTT).

---

## Topics que PUBLICA el ESP32

La app / Node-RED se **suscribe** a estos topics.

| Topic | Retain | QoS | Cuándo se publica | Payload |
|---|:---:|:---:|---|---|
| `soa/{id}/availability` | ✅ | 1 | Al conectar (`online`) y como **Last Will (LWT)** (`offline`) | texto plano |
| `soa/{id}/status` | ✅ | 0 | En cada transición de la FSM (on-change) | JSON |
| `soa/{id}/shelf/{NN}/stock` | ✅ | 0 | On-change de peso/stock/disponibilidad | JSON |
| `soa/{id}/shelf/{NN}/security` | ✅ | 0 | On-change del estado de seguridad | JSON |

`{NN}` ∈ `01` | `02`.

### `availability`

Texto plano. `online` al conectar; `offline` lo publica el broker automáticamente vía LWT si el
embebido se desconecta sin avisar.

```
online
```

### `status`

Refleja el estado de la FSM. Mapeo: `VIRGIN_EMBEDDED → IDLE`, `STOCK_MODE → STOCK`,
`SECURITY_MODE → SECURITY`. Le dice a la app qué pantalla está activa.

```json
{ "mode": "STOCK" }
```

| Campo | Tipo | Valores |
|---|---|---|
| `mode` | string | `IDLE` \| `STOCK` \| `SECURITY` |

### `shelf/{NN}/stock`

Estado de inventario de una góndola (pantalla "Inventario en Tiempo Real"). Se publica solo
**on-change** (la celda se muestrea cada 200 ms; publicar en cada muestra inundaría el broker).

```json
{ "weight": 3000, "stock": 3, "min": 1, "available": true }
```

| Campo | Tipo | Descripción | Uso en la app |
|---|---|---|---|
| `weight` | int | Peso medido total, en gramos | "Peso Actual: 3000 g" |
| `stock` | int | Unidades calculadas (`weight / unitWeight`) | — |
| `min` | int | Stock mínimo aceptable | — |
| `available` | bool | `stock >= min` | "Estado: DISPONIBLE / AGOTADO" |

### `shelf/{NN}/security`

Estado de seguridad de una góndola (pantalla "Seguridad Activa"). Se publica solo **on-change**.
`secure` refleja el latch de anomalía: `false` cuando el peso se desvió del baseline más que el umbral
(lo mismo que dispara el buzzer/LED).

```json
{ "secure": true, "baseline": 3000, "current": 3000, "delta": 0 }
```

| Campo | Tipo | Descripción | Uso en la app |
|---|---|---|---|
| `secure` | bool | `true` = sin anomalía, `false` = anomalía detectada | "Estado: SEGURO / ALERTA" |
| `baseline` | int | Peso de referencia, en gramos | — |
| `current` | int | Peso actual, en gramos | — |
| `delta` | int | `current - baseline`, en gramos | — |

---

## Topics a los que SE SUSCRIBE el ESP32

La app / Node-RED **publica** en estos topics para controlar el dispositivo de forma remota. Un
comando produce el **mismo efecto** que accionar el botón/sensor físico (es "la misma acción, pero
remota"): los botones físicos siguen funcionando igual.

| Topic | QoS | Efecto en el embebido | Payload |
|---|:---:|---|---|
| `soa/{id}/cmd/mode` | 1 | Cambiar de modo (equivale a los botones físicos) | texto plano |
| `soa/{id}/cmd/alarm` | 1 | Silenciar / reactivar el buzzer en modo Security | texto plano |
| `soa/{id}/cmd/tare` | 1 | Re-tarar / fijar baseline de las celdas | JSON |

### `cmd/mode`

```
SECURITY
```

| Valor | Efecto | Equivale a |
|---|---|---|
| `STOCK` | Activa modo Stock | Botón Stock ON, Security OFF |
| `SECURITY` | Activa modo Security (prioritario) | Botón Security ON |
| `OFF` | Vuelve a IDLE | Ambos botones OFF ("Desactivar Stock/Seguridad") |

### `cmd/alarm`

```
OFF
```

| Valor | Efecto |
|---|---|
| `OFF` | Silencia el buzzer (la alerta sigue visible en LCD/LED y en `secure:false`) |
| `ON` | Reactiva el buzzer |

> [!NOTE]
> Por diseño de "mute simple": silenciar no apaga la alerta, solo el sonido. La alerta persiste hasta
> salir de Security mode.

### `cmd/tare`

Re-tara una celda o todas, fijando el baseline al peso actual.

```json
{ "sensor": 1 }
```

| Campo | Tipo | Valores |
|---|---|---|
| `sensor` | int \| string | `1` \| `2` \| `"all"` |

---

## Credenciales / autenticación

La autenticación es **a nivel de conexión al broker**, no por topic: el embebido se autentica una
sola vez al conectar y los **ACLs** del broker deciden a qué topics puede publicar/suscribirse.

### Lado broker (Mosquitto)

- **Recomendado — usuario + contraseña.** En `mosquitto.conf`:
  ```conf
  allow_anonymous false
  password_file /mosquitto/config/passwd
  ```
  Usuario creado con:
  ```bash
  mosquitto_passwd -c /mosquitto/config/passwd esp32
  ```
- **Opcional — ACLs.** Restringir el usuario del ESP32 a su propio prefijo (`acl_file`):
  ```conf
  user esp32
  topic write soa/gondola-01/#
  topic read  soa/gondola-01/cmd/#
  ```
- Para una demo rápida sin credenciales se puede usar `allow_anonymous true` (no recomendado).

### Lado firmware (`src/secrets.h`, git-ignored)

Las credenciales **nunca** se versionan: viven en `src/secrets.h` (ignorado por git) y se pasan a
`mqttClient.connect()`. Se versiona `src/secrets.example.h` como plantilla.

| Constante | Uso |
|---|---|
| `WIFI_SSID`, `WIFI_PASS` | Red WiFi (en Wokwi: `Wokwi-GUEST`, sin contraseña) |
| `MQTT_HOST`, `MQTT_PORT` | Endpoint del broker (IP LAN del host del Docker, `1883`) |
| `MQTT_USER`, `MQTT_PASS` | Credenciales pasadas a `mqttClient.connect()` |
| `MQTT_CLIENT_ID` | Identificador único por dispositivo = `{id}` de los topics (ej. `gondola-01`) |

Conexión con autenticación + Last Will integrado (PubSubClient v2.8):

```cpp
mqttClient.connect(
    MQTT_CLIENT_ID,
    MQTT_USER, MQTT_PASS,
    "soa/gondola-01/availability", 1, true, "offline"  // LWT
);
```

---

## Diagrama del flujo

```
                     soa/{id}/status, /shelf/+/stock, /shelf/+/security, /availability
   ESP32  ─────────────────────────────────────────────────────────────────►  Mosquitto ─► Node-RED ─► App
   (FSM)  ◄─────────────────────────────────────────────────────────────────  Mosquitto ◄─ Node-RED ◄─ App
                     soa/{id}/cmd/mode, /cmd/alarm, /cmd/tare
```

---

## Pruebas rápidas

Suscribirse a todo lo que publica el dispositivo:

```bash
mosquitto_sub -h <broker> -u esp32 -P <pass> -t 'soa/#' -v
```

Forzar un cambio de modo remoto (debe reaccionar igual que el botón físico):

```bash
mosquitto_pub -h <broker> -u esp32 -P <pass> -t 'soa/gondola-01/cmd/mode' -m SECURITY
```

Silenciar el buzzer y re-tarar la celda 1:

```bash
mosquitto_pub -h <broker> -u esp32 -P <pass> -t 'soa/gondola-01/cmd/alarm' -m OFF
mosquitto_pub -h <broker> -u esp32 -P <pass> -t 'soa/gondola-01/cmd/tare'  -m '{"sensor":1}'
```
