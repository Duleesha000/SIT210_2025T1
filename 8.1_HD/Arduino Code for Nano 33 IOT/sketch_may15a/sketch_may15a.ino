#include <ArduinoBLE.h>

#define TRIG_PIN 4
#define ECHO_PIN 5

BLEService parkingService("180C");
BLEUnsignedIntCharacteristic distanceCharacteristic("2A56", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Serial ready");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if (!BLE.begin()) {
    Serial.println("BLE failed");
    while (1);
  }

  BLE.setLocalName("ParkingSensor");
  BLE.setAdvertisedService(parkingService);
  parkingService.addCharacteristic(distanceCharacteristic);
  BLE.addService(parkingService);
  distanceCharacteristic.writeValue(0);
  BLE.advertise();

  Serial.println("BLE advertising started");
}

long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("🔗 Connected to: ");
    Serial.println(central.address());

    while (central.connected()) {
      long distance = readDistanceCM();
      distanceCharacteristic.writeValue((unsigned int)distance);

      Serial.print("📏 Distance: ");
      Serial.print(distance);
      Serial.println(" cm");

      delay(1000);
    }

    Serial.println("Disconnected");
  }
}
