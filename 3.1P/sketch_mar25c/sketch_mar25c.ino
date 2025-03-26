#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>  // MQTT library

// WiFi Credentials
#define WIFI_SSID "Nimesh's A55"
#define WIFI_PASSWORD "12345667"

// MQTT Broker Credentials
#define MQTT_SERVER "8d6f19014dcb4ab598dc1e144e2703b7.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_TOPIC "sensor/light"
#define MQTT_USER "hivemq.webclient.1742892387266"
#define MQTT_PASSWORD "AHswV!i0.e82o3%UYzJ?"

// BH1750 Sensor
BH1750 lightMeter;

// Use SSL Client instead of regular WiFiClient
WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);

// Light threshold for detecting sunlight
#define LIGHT_THRESHOLD 5000  
bool isSunlightOn = false;    // Track sunlight status

// Function to Connect to MQTT Broker
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ArduinoNanoIoT", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT Broker!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize the BH1750 sensor
  Wire.begin();
  lightMeter.begin();

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set MQTT Server
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  // Connect to MQTT
  connectMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();  // Keep MQTT connection alive

  // Read the light level from the BH1750 sensor (lux)
  uint16_t lightLevel = lightMeter.readLightLevel();
  Serial.print("Light Level: ");
  Serial.print(lightLevel);
  Serial.println(" lux");

  // Detect sunlight start and stop
  if (lightLevel > LIGHT_THRESHOLD && !isSunlightOn) {
    isSunlightOn = true;
    Serial.println("Sunlight started!");
    mqttClient.publish(MQTT_TOPIC, "Sunlight Started");
  } else if (lightLevel < LIGHT_THRESHOLD && isSunlightOn) {
    isSunlightOn = false;
    Serial.println("Sunlight stopped!");
    mqttClient.publish(MQTT_TOPIC, "Sunlight Stopped");
  }

  delay(5000); // Check light level every 5 seconds
}
