#include "arduino_secrets.h"
// LED pin (D13)
#define LED_PIN 13

// Timing
#define DOT_DURATION 300   // 0.3 sec for dot
#define DASH_DURATION 1000 // 1 sec for dash
#define LETTER_GAP 1500    // 1.5 sec between letters
#define SYMBOL_GAP 300     // 0.3 sec between symbols

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

// Function to blink dot
void dot() {
  digitalWrite(LED_PIN, HIGH);
  delay(DOT_DURATION);
  digitalWrite(LED_PIN, LOW);
  delay(SYMBOL_GAP);
}

// Function to blink dash
void dash() {
  digitalWrite(LED_PIN, HIGH);
  delay(DASH_DURATION);
  digitalWrite(LED_PIN, LOW);
  delay(SYMBOL_GAP);
}

// Function to blink a letter in Morse code
void blinkLetter(const char *morse) {
  while (*morse) {
    if (*morse == '.') {
      dot();
    } else if (*morse == '-') {
      dash();
    }
    morse++;
  }
  delay(LETTER_GAP); // Wait between letters
}

void loop() {
  // Morse code for "NIMESH"
  blinkLetter("-.");   // N
  blinkLetter("..");   // I
  blinkLetter("--");   // M
  blinkLetter(".");    // E
  blinkLetter("...");  // S
  blinkLetter("...."); // H

  delay(10000); // Pause before repeating the name
}