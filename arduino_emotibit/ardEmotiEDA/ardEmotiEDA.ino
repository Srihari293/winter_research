#include "programmable_air.h"
#include <Adafruit_NeoPixel.h>
#define DEBUG 1

int state = UN_KNOWN;

int threshold = 8;
int switch_ = 9;

char val; // Data received from the serial port
int ledPin = 11; // Set the pin to digital I/O 13
bool handshakeDone = false; // Flag to indicate if the handshake is done
//EDA
float min_EDA = 10;
float max_EDA = 0;
float curr_EDA = 0;
float avg_EDA = 0;
float sum_EDA = 0;
//HR
int min_HR = 0;
int max_HR = 0;
int avg_HR = 0;
int curr_HR = 0;
int sum_HR = 0;

int i = 0;
int iterations = 300;
int change_min = 0;
int change_max = 0;

void setup() {
  initializePins();
  pinMode(switch_, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
  Serial.begin(9600); // Start serial communication at 9600 bps
  switchOffPump(1);
  switchOffPump(2);
}

void loop() {
  if (!handshakeDone) {
    establishContact(); // Perform handshake with Processing
  }

  else {
    if (Serial.available()) {
      val = Serial.read(); // Read data from Processing

      if (val == 'E') { // EDA data received
        curr_EDA = processEDAData(); // Process EDA data
        if (curr_EDA > max_EDA) {
          Serial.print("Max assigned");
          Serial.println(curr_EDA, 6);
          max_EDA = curr_EDA;
          change_max++;
        }
        else if (curr_EDA < min_EDA) {
          Serial.print("Min assigned");
          Serial.println(curr_EDA, 6);
          min_EDA = curr_EDA;
          change_min++;
        }
        if (i < iterations) {
          sum_EDA = sum_EDA + curr_EDA;
        }
        if (i == iterations) {
          avg_EDA = sum_EDA / iterations;
        }
        Serial.print("COUNTER: ");
        Serial.println(i);
        Serial.print("CURR: ");
        Serial.println(curr_EDA, 6);
        i++;
      }
      else if (val == 'H') {
        curr_HR = processHRData(); // Process HR data
        if (curr_HR > max_HR) {
          Serial.print("Max assigned");
          Serial.println(curr_HR);
          max_HR = curr_HR;
        }
        else if (curr_HR < min_HR) {
          Serial.print("Min assigned");
          Serial.println(curr_HR);
          min_HR = curr_HR;
        }
        if (i < iterations) {
          sum_HR = sum_HR + curr_HR;
        }
        if (i == iterations) {
          avg_HR = sum_HR / iterations;
        }
        Serial.print("COUNTER: ");
        Serial.println(i);
        Serial.print("CURR: ");
        Serial.println(curr_HR);
        i++;
      }

      if (i >= iterations) {
        Serial.print("Min EDA: ");
        Serial.println(min_EDA, 6);
        Serial.print("Max EDA: ");
        Serial.println(max_EDA, 6);
        Serial.print("Avg EDA: ");
        Serial.println(avg_EDA, 6);
        Serial.print("Min HR: ");
        Serial.println(min_HR);
        Serial.print("Max HR: ");
        Serial.println(max_HR);
        Serial.print("Avg HR: ");
        Serial.println(avg_HR);

        if (curr_EDA - avg_EDA > 0.8 * (max_EDA - avg_EDA)) {
          Serial.println("LIE DETECTED!");
          for (int i = 0; i < 100; i++) {
            switchOnPump(2, 100);
            switchOffPump(1);
            blow();
            state = BLOWING;
          }
        }
      }
      Serial.println("----------------------");
    }
  }
}

void establishContact() {
  while (!handshakeDone) {
    if (Serial.available()) {
      val = Serial.read();
      if (val == 'A') {
        Serial.println("A"); // Respond with 'A' to confirm contact
        handshakeDone = true; // Handshake is done
        digitalWrite(ledPin, HIGH); // Turn on the LED to indicate successful handshake
      }
    } else {
      Serial.println("A"); // Send 'A' to request contact
      delay(300);
    }
  }
}

float processEDAData() {
  // Read EDA data from Processing
  String edaDataString = Serial.readStringUntil('\n');
  Serial.print("EDA as string: ");
  Serial.println(edaDataString);
  // Remove the first character from the string
  edaDataString = edaDataString.substring(1);
  // Convert the string to a float
  float edaData = atof(edaDataString.c_str());
  // Send acknowledgement back to Processing
  Serial.print("E");
  Serial.println(edaData, 6);
  return edaData;
}

int processHRData() {
  // Read HR data from Processing
  String HRDataString = Serial.readStringUntil('\n');
  Serial.print("HR as string: ");
  Serial.println(HRDataString);
  // Remove the first character from the string
  HRDataString = HRDataString.substring(1);
  // Convert the string to an integer
  int HRData = atoi(HRDataString.c_str());
  // Send acknowledgement back to Processing
  Serial.print("H");
  Serial.println(HRData);
  return HRData;
}
