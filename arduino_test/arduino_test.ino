/*
  // Test 1 - Using arduino UNO
  int val; // Data received from the serial port
  int ledPin = 11; // Set the pin to digital I/O 13

  void setup() {
   pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
   Serial.begin(9600); // Start serial communication at 9600 bps
  }

  void loop() {
   if (Serial.available())
   { // If data is available to read,
     val = Serial.read(); // read it and store it in val
   }
   if (val%2 == 0)
   {
     digitalWrite(ledPin, HIGH); // turn the LED on
     Serial.println("Got some value");
     Serial.println(val);
   }

   else {
     digitalWrite(ledPin, LOW); // otherwise turn it off
     Serial.println("Didnt get anything");
   }

  }

*/

/*
  // Test 2 - Using the programmble air arduino nano to blink internal led
  int val; // Data received from the serial port
  int ledPin = 13; // Set the pin to digital I/O 13

  void setup() {
   pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
   Serial.begin(9600); // Start serial communication at 9600 bps
  }

  void loop() {
   if (Serial.available())
   { // If data is available to read,
     val = Serial.read(); // read it and store it in val
   }
   if (val%2 == 0)
   {
     digitalWrite(ledPin, HIGH); // turn the LED on
     Serial.println("Got some value");
     Serial.println(val);
   }

   else {
     digitalWrite(ledPin, LOW); // otherwise turn it off
     Serial.println("Didnt get anything");
   }
  }

*/

// Test 3 - Using the programmble air arduino uno to control motor with dummy data
#include "programmable_air.h"
#include <Adafruit_NeoPixel.h>
#define DEBUG 1

int state = UN_KNOWN;

int atmospheric_pressure = 508; // should be around 508
int threshold = 8;

int switch_ = 9;

int val; // Data received from the serial port
int ledPin = 13; // Set the pin to digital I/O 13


void setup() {
  initializePins();
  pinMode(switch_, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
  Serial.begin(9600); // Start serial communication at 9600 bps
}

void loop() {
  showPressure();
  if (Serial.available())
  { // If data is available to read,
    val = Serial.read(); // read it and store it in val
  }
  if (val%5 == 0)
  {
    digitalWrite(ledPin, HIGH); // turn the LED on
    Serial.println("Got some value");
    Serial.println(val);
    switchOnPump(1, 100);
    switchOffPump(2);
    blow();
    state = BLOWING;
  }

  else {
    digitalWrite(ledPin, LOW); // otherwise turn it off
    Serial.println("Didnt get anything");
    switchOffPump(1);
    switchOffPump(2);
    /*
    switchOnPump(1, 100);
    blow();
    delayWhileReadingPressure(250);
    switchOffPumps();
    vent();
    state = VENTING;
    */
  }
  // delayWhileReadingPressure(200);
}
