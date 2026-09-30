# Air Quality Sentinel 🌱

An IoT-based indoor environmental monitoring system built with an **ESP32**, **DHT22 temperature/humidity sensor**, **LDR light sensor**, **MQTT**, and a browser-based dashboard.

The system collects environmental data from the ESP32, calculates a simple indoor risk score, publishes the readings through MQTT, and displays the live data on a web dashboard. An optional AI layer can generate short, personalized recommendations based on the sensor readings.

## Features

- 🌡️ Real-time temperature monitoring
- 💧 Real-time humidity monitoring
- 💡 Ambient light monitoring
- 📊 Rule-based indoor risk scoring
- 📡 MQTT communication
- 🌐 Live browser dashboard
- 🤖 Optional AI-generated environmental advice
- 📱 Responsive dashboard layout
- 🔌 ESP32-based IoT architecture

## Architecture

```text
DHT22 ──────┐
            │
LDR ────────┤
            ▼
         ESP32
            │
            │ MQTT
            ▼
     HiveMQ MQTT Broker
            │
            ▼
    Web Dashboard
            │
            ▼
      Optional AI API
```

## Hardware

- ESP32 development board
- DHT22 temperature/humidity sensor
- LDR/photoresistor
- Appropriate resistor for the LDR circuit
- Jumper wires
- Breadboard

### Pin configuration

| Component | ESP32 pin |
|---|---:|
| DHT22 data | GPIO 15 |
| LDR analog output | GPIO 4 |

> Pin assignments can be changed in `esp32/air_quality_sentinel.ino`.

## Software

- Arduino IDE
- ESP32 Arduino core
- `DHT` library
- `PubSubClient` library
- Paho MQTT JavaScript client
- HiveMQ public MQTT broker
- Optional OpenAI API integration through a backend

## Project structure

```text
air-quality-sentinel/
├── esp32/
│   └── air_quality_sentinel.ino
├── dashboard/
│   └── index.html
├── backend/
│   ├── server.js
│   ├── package.json
│   └── .env.example
├── .gitignore
└── README.md
```

## How the risk score works

The current implementation uses a simple rule-based score.

### Humidity

- Above 65% → +45 points
- 55–65% → +25 points
- Below 30% → +20 points

### Temperature + humidity

- Temperature above 26°C AND humidity above 55% → +30 points

### Light

- Light above 75% → +25 points

### Final risk

- `0–39` → Low
- `40–69` → Medium
- `70+` → High

This is a **demonstration heuristic**, not a medical or certified indoor-air-quality measurement system.

## MQTT message

The ESP32 publishes JSON messages to:

```text
demo/simple/topic
```

Example:

```json
{
  "temperature": 24.5,
  "humidity": 61.2,
  "light": 43,
  "risk": "Medium",
  "riskScore": 25
}
```

## Setup

### 1. Install ESP32 dependencies

In Arduino IDE, install the ESP32 board package and the following libraries:

- DHT sensor library
- PubSubClient

### 2. Configure Wi-Fi

Open:

```text
esp32/air_quality_sentinel.ino
```

Replace:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

with your local Wi-Fi credentials.

**Do not commit real Wi-Fi credentials to GitHub.**

### 3. Upload the firmware

Select your ESP32 board and upload:

```text
esp32/air_quality_sentinel.ino
```

Open Serial Monitor at:

```text
115200 baud
```

You should see the Wi-Fi connection and sensor readings.

### 4. Run the complete application

The easiest approach is to run the included backend, because it serves the dashboard and provides the AI endpoint.

From the project root:

```bash
cd backend
npm install
```

Create `.env`:

```env
OPENAI_API_KEY=your_openai_api_key_here
PORT=3000
```

Start the application:

```bash
npm start
```

Then open:

```text
http://localhost:3000
```

The dashboard connects to HiveMQ using MQTT over WebSockets on port `8000`.

If you only want to test the dashboard without AI, you can also open `dashboard/index.html` through a local web server. The AI advice will not work unless the backend is running.

## AI integration

The project includes a Node.js backend so the OpenAI API key is **never exposed in browser JavaScript**.

The flow is:

```text
Dashboard
   │
   │ POST /api/advice
   ▼
Node.js / Express
   │
   │ OpenAI API
   ▼
AI-generated advice
   │
   ▼
Dashboard
```

### Backend setup

From the project root:

```bash
cd backend
npm install
```

Create a `.env` file based on `.env.example`:

```env
OPENAI_API_KEY=your_openai_api_key_here
PORT=3000
```

Then start the server:

```bash
npm start
```

Open:

```text
http://localhost:3000
```

The backend serves the dashboard and exposes:

```text
POST /api/advice
```

The API key remains on the server and is never sent to the browser.

**Never commit `.env` to GitHub.** The `.gitignore` already excludes it.

### Production note

The public HiveMQ broker is used for demonstration purposes. For a real deployment, use an authenticated MQTT broker with TLS and consider adding authentication and rate limiting to the backend.

## Important limitations

This project is an educational IoT prototype.

The calculated risk score does **not** measure actual air quality and should not be treated as a medical diagnosis or certified allergy assessment.

The DHT22 measures temperature and relative humidity. The LDR measures light. Neither sensor directly measures:

- PM2.5
- PM10
- CO₂
- VOCs
- pollen
- mold
- allergens

For a more complete indoor air-quality system, future versions could add dedicated particulate, CO₂, VOC, and environmental sensors.

## Future improvements

- [ ] Add a CO₂ sensor
- [ ] Add PM2.5/PM10 monitoring
- [ ] Add VOC monitoring
- [ ] Store historical sensor data
- [ ] Add charts and historical trends
- [ ] Add alerts when risk increases
- [ ] Add a proper backend for AI recommendations
- [ ] Replace the public MQTT broker with an authenticated broker
- [ ] Add MQTT TLS
- [ ] Add multiple rooms/devices
- [ ] Add user authentication
- [ ] Deploy the dashboard online

## Technologies

**Hardware**

`ESP32` · `DHT22` · `LDR`

**IoT / Messaging**

`MQTT` · `HiveMQ` · `PubSubClient`

**Frontend**

`HTML` · `CSS` · `JavaScript` · `Paho MQTT`

**AI**

`OpenAI API` · `REST API`

## GitHub description

> ESP32-based indoor environmental monitoring system using DHT22, LDR, MQTT, and a real-time web dashboard with optional AI-powered recommendations.

## Topics

`IoT` `ESP32` `MQTT` `Arduino` `DHT22` `Smart Home` `Environmental Monitoring` `JavaScript` `OpenAI` `Web Dashboard`
