# Infrastructure

## Mosquitto

Mosquitto is a message broker that implements the MQTT protocol, allowing devices to communicate with each other by publishing and subscribing to topics. In this project, Mosquitto is connected to the ESP32 using the MQTT protocol, enabling real-time data exchange and control commands between devices through Node-RED.

### Topics published by ESP32

| Topic                 | Description                              | When it is published                                         |
| --------------------- | ---------------------------------------- | ------------------------------------------------------------ |
| `{id}/health`         |                                          | On connect (`online`) and as **Last Will (LWT)** (`offline`) |
| `{id}/status`         | Reports the operating mode of the ESP32. | On every FSM transition (on-change)                          |
| `{id}/shelf/stock`    | Reports the status of the stock          | On-change of weight/stock/health                             |
| `{id}/shelf/security` | Reports the status of the stock          | On-change of the security state                              |

### Request and responses examples

<details>
<summary><code>gondola-01/status</code></summary>

```jsonc
// Response...
{
  "stock": true, // boolean
  "security": true, // boolean
  "active": "security" // "none" | "stock" | "security"
}
```

</details>

<details>
<summary><code>gondola-01/shelf/stock</code></summary>

```jsonc
// Response...
{
  "available": true, // boolean - Derived from `stock >= min`
  "min": 1, // int - Minimum stock threshold
  "stock": 3, // int - Derived from weight and unit weight (e.g., 3000 g / 1000 g per unit = 3 units)
  "weight": 3000 // int - Current weight (in grams)
}
```

</details>

<details>
<summary><code>gondola-01/shelf/security</code></summary>

```jsonc
// Response...
{
  "baseline": 3000, // int - Reference weight (in grams)
  "current": 3000, // int - Current weight (in grams)
  "secure": true // boolean - `true` = no anomaly, `false` = anomaly detected
}
```

</details>

## Node-RED

Node-RED is a flow-based development tool for visual programming, used to connect and orchestrate various services and devices. In this project, Node-RED serves as the central hub for processing data from the ESP32 and sending commands back to it via MQTT, and vice versa.

### Endpoints

| Endpoint                            | Description                        | Request                  |
| ----------------------------------- | ---------------------------------- | ------------------------ |
| `GET {host}/api/{id}/state`         | Get state of `{id}`.               |                          |
| `POST {host}/api/{id}/cmd/stock`    | Activate/Deactivate stock mode.    | `{"on": bool}`           |
| `POST {host}/api/{id}/cmd/security` | Activate/Deactivate security mode. | `{"on": bool}`           |
| `POST {host}/api/{id}/cmd/alarm`    | Play/Mute the alarm.               | `{"alarm": "ON"\|"OFF"}` |

#### Request and responses examples

<details>
<summary><code>GET http://localhost:1880/api/gondola-01/state</code></summary>

```jsonc
// Response...
{
  "availability": "online",
  "status": {
    "stock": false,
    "security": false,
    "active": "IDLE"
  },
  "shelf": {
    "01": {
      "stock": {
        "weight": 0,
        "stock": 0,
        "min": 1,
        "available": false
      },
      "security": {
        "secure": true,
        "baseline": 0,
        "current": 0,
        "delta": 0
      }
    }
  },
  "updatedAt": 1781553219269,
  "tare": {
    "01": {
      "done": true,
      "ts": 1781551801990
    },
    "02": {
      "done": false
    }
  }
}
```

</details>

<details>
<summary><code>POST http://localhost:1880/api/gondola-01/cmd/stock</code></summary>

```jsonc
// Request...
{
  "on": true
}

// Response...
{
  "ok": true,
  "topic": "gondola-01/cmd/stock",
  "payload": "ON"
}
```

</details>

<details>
<summary><code>POST http://localhost:1880/api/gondola-01/cmd/security</code></summary>

```jsonc
// Request...
{
  "on": true
}

// Response...
{
  "ok": true,
  "topic": "gondola-01/cmd/security",
  "payload": "OFF"
}
```

</details>

<details>
<summary><code>POST http://localhost:1880/api/gondola-01/cmd/alarm</code></summary>

```jsonc
// Request...
{
  "alarm": "ON"
}

// Response...
{
  "ok": true,
  "topic": "gondola-01/cmd/alarm",
  "payload": "OFF"
}
```

</details>

## How to test manually Mosquitto topics and Node-RED endpoints

1. Inside the DevContainer, open a terminal and execute `docker compose up --file infrastructure/compose.yaml --detach` to start Mosquitto and Node-RED services.
<!-- TODO: Cómo testear los tópico de Mosquitto  -->
2. To test Node-RED endpoints, open [infrastructure/endpoints.http](../infrastructure/endpoints.http) file and click on `Send Request` (above each endpoint) provided by the REST Client extension in VSCode. If you need, you can change declared variables at the top of the file (e.g., `hostname` or `id`) to match your testing environment.
