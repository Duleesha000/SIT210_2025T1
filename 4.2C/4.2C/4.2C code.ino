// Pin Assignments 
const int buttonPin = 2;     // Push button
const int sensorPin = 3;     // Motion sensor
const int led1Pin = 8;       // LED for button
const int led2Pin = 9;       // LED for sensor

// States 
bool led1State = LOW;
bool led2State = LOW;

// For button debounce 
bool lastButtonState = HIGH;         // Button unpressed
unsigned long lastButtonPress = 0;
unsigned long buttonDebounceDelay = 50;  // 50ms

// For motion cooldown 
unsigned long lastMotionTime = 0;
unsigned long motionCooldown = 10000;  // 5 seconds

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Use internal pull-up
  pinMode(sensorPin, INPUT);         // PIR sensor
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  Serial.begin(9600);
  Serial.println("System Ready – No Interrupts Used");
}

void loop() {
  // Handle Button Press (Toggle LED1) 
  bool buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastButtonPress > buttonDebounceDelay)) {
    led1State = !led1State;
    digitalWrite(led1Pin, led1State);
    Serial.print("Button Pressed - LED1 toggled: ");
    Serial.println(led1State);
    lastButtonPress = millis();
  }
  lastButtonState = buttonState;

  // Handle Motion Sensor (Toggle LED2 with Cooldown) 
  int sensorValue = digitalRead(sensorPin);

  if (sensorValue == HIGH && (millis() - lastMotionTime > motionCooldown)) {
    led2State = !led2State;
    digitalWrite(led2Pin, led2State);
    Serial.print("Motion Detected - LED2 toggled: ");
    Serial.println(led2State);
    lastMotionTime = millis();
  }

  delay(50); // Small delay to reduce Serial flooding
}
