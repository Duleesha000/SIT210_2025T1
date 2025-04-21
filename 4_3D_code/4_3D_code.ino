#include <DHT.h>

// Pin Assignments
#define BUTTON_PIN 2     // Button (Interrupt)
#define DHT_PIN    3     // DHT11 Sensor
#define TRIG_PIN   4     // Ultrasonic TRIG
#define ECHO_PIN   5     // Ultrasonic ECHO
#define LED1_PIN   6     // LED1 - Button press
#define LED2_PIN   7     // LED2 - Object detected < 20cm
#define LED3_PIN   8     // LED3 - Blinks every second

#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// States
volatile bool led1State = LOW;
bool led2State = LOW;
bool led3State = LOW;

// Timing for millis
unsigned long previousMillis = 0;
const long interval = 1000; // 1 second

// Interrupt: Button Press
void handleButtonInterrupt() {
  led1State = !led1State;
  digitalWrite(LED1_PIN, led1State);
  Serial.print("Button Pressed - LED1: ");
  Serial.println(led1State);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Set pin modes
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize DHT11
  dht.begin();

  // Button interrupt setup
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonInterrupt, FALLING);

  Serial.println(" System initialized using millis()");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Blink LED3 every second
    led3State = !led3State;
    digitalWrite(LED3_PIN, led3State);
    Serial.println("millis: LED3 toggled");

    //  DHT11 Sensor Reading
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (!isnan(temp) && !isnan(hum)) {
      Serial.print("DHT11 - Temp: ");
      Serial.print(temp);
      Serial.print(" °C, Humidity: ");
      Serial.print(hum);
      Serial.println(" %");
    } else {
      Serial.println("DHT11 read failed");
    }

    // Ultrasonic Distance Measurement
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 20000);
    float distance = duration * 0.034 / 2.0;

    Serial.print("Ultrasonic Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    //  LED2 - Object Detection
    if (distance > 0 && distance < 20) {
      digitalWrite(LED2_PIN, HIGH);
      Serial.println("Object detected – LED2 ON");
    } else {
      digitalWrite(LED2_PIN, LOW);
    }
  }
}
