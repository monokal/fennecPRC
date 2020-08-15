#include "BTS7960.h"
#include "Arduino.h"

// Constructor.
BTS7960::BTS7960(int RPWM, int R_EN, int R_IS, int LPWM, int L_EN, int L_IS,
                 bool DEBUG) {
  _debug = DEBUG;

  // Get pins.
  _RPWM = RPWM;
  _R_EN = R_EN;
  _R_IS = R_IS;

  _LPWM = LPWM;
  _L_EN = L_EN;
  _L_IS = L_IS;
}

// Begin function.
void BTS7960::begin() {
  // Setup pins.
  pinMode(this->_RPWM, OUTPUT);
  pinMode(this->_R_EN, OUTPUT);
  // pinMode(this->_R_IS, OUTPUT);

  pinMode(this->_LPWM, OUTPUT);
  pinMode(this->_L_EN, OUTPUT);
  // pinMode(this->_L_IS, OUTPUT);

  if (this->_debug) {
    Serial.println("H-Bridge > Initialised.");
  }
}

// Start function.
void BTS7960::start(int percent, int direction) {
  int percentPWM = toPWM(percent);

  if (percent >= 0 && percent <= 100) {
    // Enable all.
    digitalWrite(this->_R_EN, HIGH);
    digitalWrite(this->_L_EN, HIGH);

    // Right.
    if (direction == 0) {
      analogWrite(this->_RPWM, percentPWM);
    }
    // Left.
    else if (direction == 1) {
      analogWrite(this->_LPWM, percentPWM);
    }
    // Invalid.
    else {
      if (this->_debug) {
        Serial.println("H-Bridge > Invalid direction.");
      }
    }
  } else {
    if (this->_debug) {
      Serial.println("H-Bridge > Invalid percentage.");
    }
  }
  if (this->_debug) {
    Serial.println("H-Bridge > Started.");
  }
}

// Stop function.
void BTS7960::stop() {
  digitalWrite(this->_R_EN, LOW);
  digitalWrite(this->_L_EN, LOW);

  if (this->_debug) {
    Serial.println("H-Bridge > Stopped.");
  }
}

// Converts a percentage (0-100) to 0-255 for PWM.
int BTS7960::toPWM(int percent) { return map(percent, 0, 100, 0, 255); }
