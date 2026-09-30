#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Wi-Fi
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "demo/simple/topic";

// Sensors
#define DHTPIN 15
#define DHTTYPE DHT22
#define LDR_PIN 4

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println("\nAir Quality Sentinel starting...");

  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    yield();
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int rawLight = analogRead(LDR_PIN);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT error!");
    delay(2000);
    return;
  }

  // Convert ESP32 ADC value to a simple 0-100 light scale.
  int light = map(rawLight, 0, 4095, 100, 0);
  light = constrain(light, 0, 100);

  // Simple rule-based indoor risk score.
  int score = 0;
  String risk = "Low";

  if (humidity > 65) {
    score += 45;
  } else if (humidity > 55) {
    score += 25;
  } else if (humidity < 30) {
    score += 20;
  }

  if (temperature > 26 && humidity > 55) {
    score += 30;
  }

  if (light > 75) {
    score += 25;
  }

  if (score >= 70) {
    risk = "High";
  } else if (score >= 40) {
    risk = "Medium";
  }

  Serial.printf(
    "T:%.1f°C H:%.1f%% L:%d%% (raw:%d) Risk:%s (%d)\n",
    temperature,
    humidity,
    light,
    rawLight,
    risk.c_str(),
    score
  );

  char json[160];

  snprintf(
    json,
    sizeof(json),
    "{\"temperature\":%.1f,\"humidity\":%.1f,\"light\":%d,\"risk\":\"%s\",\"riskScore\":%d}",
    temperature,
    humidity,
    light,
    risk.c_str(),
    score
  );

  if (client.connected()) {
    client.publish(mqtt_topic, json);
    Serial.println("Published: " + String(json));
  } else {
    Serial.println("MQTT skipped");
  }

  delay(5000);
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "S3-" + String(random(0xffff), HEX);

    Serial.print("Connecting to MQTT... ");

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, state=");
      Serial.println(client.state());
      delay(4000);
    }
  }
}
