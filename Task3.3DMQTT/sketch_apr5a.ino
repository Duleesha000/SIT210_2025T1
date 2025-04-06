#include <WiFiNINA.h>
#include <PubSubClient.h>

// ====== WiFi credentials ======
const char* ssid = "Nimesh's A55";
const char* password = "12345678";

// ====== MQTT broker (secured) ======
const char* mqtt_server = "w29d812d.ala.eu-central-1.emqxsl.com";
const int mqtt_port = 8883;
const char* mqtt_user = "nimesh01";               // Your EMQX username
const char* mqtt_pass = "123456";  // Your EMQX password
const char* mqtt_topic = "SIT210/wave";

// ====== Hardware pins ======
const int trigPin = 9;
const int echoPin = 10;
const int ledPin = 7;

// Use WiFiSSLClient for secure (TLS) connection
WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

unsigned long lastWaveTime = 0;
const unsigned long waveCooldown = 3000;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
  detectWave();
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT securely...");
    if (client.connect("arduinoClient", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("]: ");
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.trim();         // white space remove
  message.toLowerCase();  // lowercase එකට convert

  Serial.println(message); // Debug print

  if (message == "wave") {
    Serial.println("Wave pattern");
    // Blink 3 times slow
    for (int i = 0; i < 3; i++) {
      digitalWrite(ledPin, HIGH);
      delay(300);
      digitalWrite(ledPin, LOW);
      delay(300);
    }
  } else if (message == "pat") {
    Serial.println("Pat pattern");
    // Blink 5 times fast
    for (int i = 0; i < 5; i++) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }
  } else {
    Serial.println("Unknown message");
  }
}


void detectWave() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  if (distance > 0 && distance < 20 && millis() - lastWaveTime > waveCooldown) {
    Serial.println("Wave detected!");
    client.publish(mqtt_topic, "wave");
    lastWaveTime = millis();
  }
}