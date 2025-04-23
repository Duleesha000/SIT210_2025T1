#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials
const char* ssid = "Nimesh's A55";
const char* password = "12345678";

// Firebase setup
const char* firebaseHost = "arduinoledcontrol-84707-default-rtdb.firebaseio.com";
const int httpsPort = 443;

WiFiSSLClient wifi; // HTTPS connection
HttpClient client = HttpClient(wifi, firebaseHost, httpsPort);

// LED Pins
const int RED_LED = 2;
const int GREEN_LED = 3;
const int BLUE_LED = 4;

// State tracking
bool lastStateRed = false;
bool lastStateGreen = false;
bool lastStateBlue = false;

// Time sync (required for HTTPS)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

void setup() {
  Serial.begin(9600);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
  }
  Serial.println(" connected!");

  // Time sync
  timeClient.begin();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Serial.println("Time synced!");
}

void loop() {
  updateLED("red", RED_LED, lastStateRed);
  updateLED("green", GREEN_LED, lastStateGreen);
  updateLED("blue", BLUE_LED, lastStateBlue);

  delay(300); // 0.3s polling interval
}

void updateLED(String color, int pin, bool &lastState) {
  String path = "/" + color + ".json";
  client.get(path);

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  bool currentState = (response == "true");

  if (statusCode == 200 && currentState != lastState) {
    Serial.print(color);
    Serial.print(" status changed to: ");
    Serial.println(currentState ? "true" : "false");

    digitalWrite(pin, currentState ? HIGH : LOW);
    lastState = currentState;
  }
}
