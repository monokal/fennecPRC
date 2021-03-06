/*
  OpenPRC | Main.
  Programmable Recrystallization Chamber

  Pins:
    0  = Serial RX (debug)
    1  = Serial TX (debug)
    2  = HBRIDGE_L_EN
    3  = HBRIDGE_LPWM (PWM)
    4  = LCD_D4
    5  = LCD_D5 (PWM)
    6  = LCD_D6 (PWM)
    7  = LCD_D7
    8  = LCD_RS
    9  = LCD_EN (PWM)
    10 = LCD_BL (PWM)
    11 = HBRIDGE_RPWM (PWM)
    12 = HBRIDGE_R_EN
    13 = TEMPERATURE_PIN
    A0 = LCD shield buttons
    A1 = BUZZER
    A2 = LED (green)
*/

#include "src/libraries/BTS7960/BTS7960.h"
// #include <Adafruit_NeoPixel.h>
// #include "src/programs/Programs.h" // TODO.
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <OneWire.h>

const float VERSION = 1.0;

const bool DEBUG = true;
String command;

// Disable the buzzer.
const bool SILENT = false;

/*
  Temperature sensor config.
*/

// Values: 9 (0.5C), 10 (0.25C), 11 (0.125C) or 12 (0.0625C) bits.
const int TEMPERATURE_PRECISION = 10;
const int TEMPERATURE_PIN = 13;

OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometerDeviceAddress;

/*
  H-bridge (for TEC) config.
*/

// Define pins.
const int HBRIDGE_RPWM = 11;
const int HBRIDGE_R_EN = 12;
const int HBRIDGE_R_IS = 99; // TODO: Dummy value. R_IS not used.

const int HBRIDGE_LPWM = 3;
const int HBRIDGE_L_EN = 2;
const int HBRIDGE_L_IS = 99; // TODO: Dummy value. L_IS not used.

// H-Bridge "direction".
const int TEC_COOL = 0;
const int TEC_HEAT = 1;

BTS7960 tec(HBRIDGE_RPWM, HBRIDGE_R_EN, HBRIDGE_R_IS, HBRIDGE_LPWM,
            HBRIDGE_L_EN, HBRIDGE_L_IS, DEBUG);

/*
  LCD shield config.
*/
const int LCD_RS = 8;
const int LCD_EN = 9;
const int LCD_D4 = 4;
const int LCD_D5 = 5;
const int LCD_D6 = 6;
const int LCD_D7 = 7;

const int LCD_BL = 10; // Backlight.

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

/*
  Buzzer config.
*/
const int BUZZER = A1;

/*
  NeoPixel config.
*/
// const int NEOPIXEL_PIN = A2;
// const int NEOPIXEL_PIXELS = 24;
//
// Adafruit_NeoPixel neopixels(NEOPIXEL_PIXELS, NEOPIXEL_PIN,
//                             NEO_GRB + NEO_KHZ800);

/*
  LEDs config.
*/
const int LED_GREEN = A2;

/*
  Setup.
*/
void setup() {
  if (DEBUG) {
    Serial.begin(9600);

    Serial.println("--------------------------------------");
    Serial.print("OpenPRC v");
    Serial.println(VERSION, 1);
    Serial.println("Programmable Recrystallization Chamber");
    Serial.println("\nSerial commands: reset, program1");
    Serial.println("--------------------------------------");
  }

  /*
    Initialise buzzer.
  */
  pinMode(BUZZER, OUTPUT);
  buzzer("ok");

  /*
    Initialise NeoPixels.
  */
  // neopixels.begin();
  // neopixels.clear();
  // neopixels.setBrightness(64);
  // ledRing("error");

  /*
    Initialise LEDs.
  */
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);

  /*
    Initialise thermometer.
  */
  sensors.begin();
  sensors.getAddress(thermometerDeviceAddress, 0);
  sensors.setResolution(thermometerDeviceAddress, TEMPERATURE_PRECISION);
  if (DEBUG) {
    Serial.println("Thermometer > Initialised.");
  }

  /*
    Initialise H-Bridge/TEC.
  */
  tec.begin();
  tec.stop(); // Stop on reset.

  /*
    Initialise LCD.
  */
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("  OpenPRC v");
  lcd.print(VERSION, 1);

  lcd.setCursor(0, 1);
  lcd.print("  READY...");

  if (DEBUG) {
    Serial.println("LCD > Initialised.");
    Serial.println("OpenPRC > Ready.");
  }
}

/*
  Main loop.
*/
void loop() {
  // Listen for serial commands.
  if (DEBUG) {
    serialCommands();
  }

  /*
    Push-buttons.
    TODO: Menu system to select program.
  */
  int button = analogRead(0);

  // Right.
  if (button < 60) {
    if (DEBUG) {
      Serial.println("Button > Right pressed.");
    }
  }
  // Up.
  else if (button < 200) {
    if (DEBUG) {
      Serial.println("Button > Up pressed.");
    }
  }
  // Down.
  else if (button < 400) {
    if (DEBUG) {
      Serial.println("Button > Down pressed.");
    }
  }
  // Left.
  else if (button < 600) {
    if (DEBUG) {
      Serial.println("Button > Left pressed.");
    }
  }
  // Select.
  else if (button < 800) {
    if (DEBUG) {
      Serial.println("Button > Select pressed.");
    }
    program1();
  }
}

/*
  Soft-reset.
*/
void (*resetFunc)(void) = 0;

/*
  Serial input commands.
*/
void serialCommands() {
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    command.trim();

    // "reset" command.
    if (command.equals("reset")) {
      resetFunc();
    } else if (command.equals("program1")) {
      program1();
    } else {
      if (DEBUG) {
        Serial.println("OpenPRC > Invalid serial command.");
      }
    }
  }
}

/*
  Buzzer.
*/
void buzzer(String type) {
  if (!SILENT) {
    // Error - 3 long A4 notes.
    if (type == "error") {
      for (int i = 0; i < 3; i++) {
        tone(BUZZER, 440, 1000);
        delay(2000);
        noTone(BUZZER);
      }
      // OK - 1 short B7 note.
    } else if (type == "ok") {
      tone(BUZZER, 3951, 250);
      // Countdown - Error then OK.
    } else if (type == "countdown") {
      buzzer("error");
      buzzer("ok");
    } else {
      if (DEBUG) {
        Serial.println("OpenPRC > Invalid buzzer type.");
      }
    }
  }
}

/*
  NeoPixel LED ring.
*/
// void ledRing(String type) {
//   // Error
//   if (type == "error") {
//     for (int pixel = 0; pixel < NEOPIXEL_PIXELS; pixel++) {
//       neopixels.setPixelColor(pixel, 0, 255, 0);
//       neopixels.show();
//     }
//   }
// }

/*
  Program 1 (slow recrystallization)
  Ambient temperature minus 1C per hour until 0C.
*/
void program1() {
  // LCD/Buzzer/LED acknowledgement.
  lcd.setCursor(0, 0);
  lcd.print("Starting (P1)...");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  buzzer("countdown");
  digitalWrite(LED_GREEN, HIGH);

  lcd.setCursor(0, 0);
  lcd.print("Running (P1)... ");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");

  if (DEBUG) {
    Serial.println("P1 > Running Program 1 (P1)...");
  }

  // Get initial temperature.
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  // Start cooling at 100% power.
  tec.start(100, TEC_COOL);

  // Run until temperature hits 0C.
  while (tempC > 0) {
    // Listen for serial commands.
    serialCommands();

    /*
        TODO: Drop temperature by 1C per hour.
      */

    if (tempC != DEVICE_DISCONNECTED_C) {
      lcd.setCursor(6, 1);
      lcd.print("    ");
      lcd.print(tempC);
      lcd.print("C");

      if (DEBUG) {
        Serial.print("P1 > Temperature: ");
        Serial.print(tempC);
        Serial.println("C");
      }
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Temp read error!");

      if (DEBUG) {
        Serial.println("Thermometer > Error reading temperature!");
      }

      buzzer("error");
    }
    // Wait a little before looping.
    delay(1000); // 1s

    // Update temperature.
    sensors.requestTemperatures();
    tempC = sensors.getTempCByIndex(0);
  }
  // Stop when temperature hits 0.
  tec.stop();
  if (DEBUG) {
    Serial.println("P1 > Reached 0C.");
    buzzer("ok");
  }
}
