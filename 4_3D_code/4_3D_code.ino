#define USING_TIMER_TC3 true  // For SAMD21/SAMD51, use TC3
//=== Libraries ===
#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"

// === Pin definitions (Updated) ===
#define BTN_PIN         2   // Push button pin (interrupt)
#define PIR_SENSOR_1    3   // Motion Sensor 1 pin (interrupt)
#define PIR_SENSOR_2    13   // Motion Sensor 2 pin (interrupt)

#define LED_RED         8   // LED 3 - RED (Timer Blink)
#define LED_BLUE        6   // LED 1 - BLUE (Button)
#define LED_GREEN       7   // LED 2 - GREEN (Motion Sensor 1)


// === Global Variables ===
bool ledState = false;

#define HW_TIMER_INTERVAL_MS 10
#define SELECTED_TIMER TIMER_TC3
#define TIMER_INTERVAL_1S 1000L   // 1 second

SAMDTimer ITimer(SELECTED_TIMER);
SAMD_ISR_Timer ISR_Timer;

// === Interrupt Handlers ===
void TimerHandler(void) {
  ISR_Timer.run();  // Executes all timers
}

void BLINK_LED_RED() {
  bool state = !digitalRead(LED_RED);
  digitalWrite(LED_RED, state);
  if (state) {
    Serial.println(F("Red LED Toggled ON"));
  } else {
    Serial.println(F("Red LED Toggled OFF"));
  }
}

void MotionInterrupt_1() {
  ledState = !ledState;
  digitalWrite(LED_GREEN, ledState);
  Serial.println("Motion Sensor 1 Interrupt: GREEN LED toggled");
}

void MotionInterrupt_2() {
  Serial.println("Motion Sensor 2 Interrupt detected");
}

void ButtonInterrupt() {
  ledState = !ledState;
  digitalWrite(LED_BLUE, ledState);
  Serial.println("Button Interrupt: Blue LED toggled");
}

// === Setup ===
void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  delay(100);

  Serial.print("Using Timer: ");
  Serial.println(SELECTED_TIMER);

  // Output pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Input pins
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(PIR_SENSOR_1, INPUT);
  pinMode(PIR_SENSOR_2, INPUT);

  // Start hardware timer
  if (ITimer.attachInterruptInterval_MS(HW_TIMER_INTERVAL_MS, TimerHandler)) {
    Serial.print(F("Starting ITimer OK, millis() = "));
    Serial.println(millis());
  } else {
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
  }

  // Secondary timer: 1s LED toggle
  ISR_Timer.setInterval(TIMER_INTERVAL_1S, BLINK_LED_RED);

  // External interrupts
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), ButtonInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_1), MotionInterrupt_1, RISING);
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_2), MotionInterrupt_2, RISING);
}

// === Loop ===
void loop() {
  // Nothing to do in loop - everything handled via interrupts
}
