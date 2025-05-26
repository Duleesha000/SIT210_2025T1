#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Adafruit_CCS811.h>
#include <DHT.h>

// === Wi-Fi Credentials ===
const char* ssid = "Nimesh's A55";
const char* password = "12345678";

// === EMQX Cloud Credentials ===
const char* mqtt_server = "o321250b.ala.eu-central-1.emqxsl.com";
const int mqtt_port = 8883;
const char* mqtt_user = "user";     // your EMQX user
const char* mqtt_pass = "1234";     // your EMQX password
const char* mqtt_topic = "smartroom/airquality";

// === MQTT Client Over TLS ===
WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

// === CCS811 Sensor ===
Adafruit_CCS811 ccs;

// === DHT11 Sensor ===
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("Nano33Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  dht.begin();
  if (!ccs.begin()) {
    Serial.println("⚠️ CCS811 not found!");
    while (1);
  }

  while (!ccs.available()) {
    delay(100);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temp = dht.readTemperature();
  float humid = dht.readHumidity();

  if (isnan(temp) || isnan(humid)) {
    Serial.println("❌ DHT11 failed!");
    return;
  }

  if (ccs.available()) {
    if (!ccs.readData()) {
      int eco2 = ccs.geteCO2();
      int tvoc = ccs.getTVOC();

      String payload = "{";
      payload += "\"temperature\":" + String(temp, 1) + ",";
      payload += "\"humidity\":" + String(humid, 1) + ",";
      payload += "\"eco2\":" + String(eco2) + ",";
      payload += "\"tvoc\":" + String(tvoc);
      payload += "}";

      client.publish(mqtt_topic, payload.c_str());
      Serial.println("Published: " + payload);
    } else {
      Serial.println("❌ CCS811 read failed");
    }
  }

  delay(5000);
}
