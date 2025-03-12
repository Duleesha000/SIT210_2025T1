#include <WiFiNINA.h>       
#include "secrets.h"        
#include "ThingSpeak.h"
#include <DHT.h>            

// WiFi credentials from secrets.h
char ssid[] = SECRET_SSID;   
char pass[] = SECRET_PASS;   

// ThingSpeak credentials from secrets.h
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// WiFi Client
WiFiClient client;

// DHT Sensor Configuration
#define DHTPIN 2        // Pin connected to the DHT sensor
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

// Variables
float temperature = 0.0;
float humidity = 0.0;
String myStatus = "";

void setup() {
    Serial.begin(115200);
    while (!Serial);  // Wait for Serial Monitor

    Serial.println("Initializing DHT sensor...");
    dht.begin();      // Start the DHT sensor

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, pass);
    
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(2000);
    }
    Serial.println("\nConnected to WiFi!");

    // Start ThingSpeak
    ThingSpeak.begin(client);
}

void loop() {
    // Reconnect to WiFi if disconnected
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected! Reconnecting...");
        while (WiFi.status() != WL_CONNECTED) {
            WiFi.begin(ssid, pass);
            delay(5000);
        }
        Serial.println("Reconnected!");
    }

    // Read data from the DHT sensor
    temperature = dht.readTemperature();  // Celsius
    humidity = dht.readHumidity();

    // Check if reading is valid
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Set fields with actual sensor values
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);

    // Determine status message
    if (temperature > 30) {
        myStatus = "Warning: High Temp!";
    } else {
        myStatus = "Temperature Normal";
    }
    ThingSpeak.setStatus(myStatus);

    // Send data to ThingSpeak
    int response = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (response == 200) {
        Serial.println("ThingSpeak update successful!");
    } else {
        Serial.println("Failed to update ThingSpeak. HTTP Error: " + String(response));
    }

    // Wait 20 seconds before sending new data
    delay(20000);
}
