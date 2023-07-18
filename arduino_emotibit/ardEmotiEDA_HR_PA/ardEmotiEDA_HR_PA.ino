#include "programmable_air.h"
#include <Adafruit_NeoPixel.h>
#define DEBUG 1

int state = UN_KNOWN;

int threshold = 8;
int switch_ = 9;

char val; // Data received from the serial port
int i = 0;
int iterations = 300;

float min_EDA = 10;
float max_EDA = 0;
float curr_EDA = 0;
float avg_EDA = 0;
float sum_EDA = 0;

int change_min = 0;
int change_max = 0;

int ledPin = 11; // Set the pin to digital I/O 13
bool handshakeDone = false; // Flag to indicate if the handshake is done

void setup() {
  initializePins();
  pinMode(switch_, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
  Serial.begin(9600); // Start serial communication at 9600 bps
  switchOffPump(1);
  switchOffPump(2);
}

void loop() {
  if (!handshakeDone) 
  {
    establishContact(); // Perform handshake with Processing
  }

  else 
  {
    if (Serial.available()) 
    {
      val = Serial.read(); // Read data from Processing
      
      if (val == 'E') // EDA data received
      {
        curr_EDA = processEDAData(); // Process EDA data
        if (curr_EDA > max_EDA) 
        {
          Serial.print("Max assigned");
          Serial.println(curr_EDA, 6);
          max_EDA = curr_EDA;
          change_max++;
        }
        else if (curr_EDA < min_EDA)
        {
          Serial.print("Min assigned");
          Serial.println(curr_EDA, 6);
          min_EDA = curr_EDA;
          change_min++;
        }
        
        if (i < iterations)
        {
          sum_EDA = sum_EDA + curr_EDA;
        }
        
        if (i == iterations)
        {
          avg_EDA = sum_EDA / iterations;
        }
        
        Serial.print("COUNTER: ");
        Serial.println(i);
        Serial.print("CURR: ");
        Serial.println(curr_EDA, 6);
        
        //Serial.print("MAX: ");
        //Serial.println(max_EDA,6);
        //Serial.print("MIN: ");
        //Serial.println(min_EDA,6);
        i++;
      }
      
      //Serial.print("counter: ");
      //Serial.println(i);
      
      if (i >= iterations)
      {
        Serial.print("Min EDA: ");
        Serial.println(min_EDA, 6);
        Serial.print("Max EDA: ");
        Serial.println(max_EDA, 6);
        //Serial.println(change_min);
        //Serial.println(change_max);
        Serial.print("Avg EDA: ");
        Serial.println(avg_EDA, 6);

        if (curr_EDA - avg_EDA > 0.8 * (max_EDA - avg_EDA))
        {
          Serial.println("LIE DETECTED!");
          for (int i = 0; i < 100; i++)
          {
            switchOnPump(2, 100);
            switchOffPump(1);
            blow();
            state = BLOWING;
          }
          state = UN_KNOWN;
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
      //Serial.print("VAL: ");
      //Serial.println(val);
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
  //Serial.println("----------------------");
  Serial.print("EDA as string: ");
  Serial.println(edaDataString);
  //edaDataString=edaDataString.remove(0,1);
  // Convert the string to a float
  float edaData = atof(edaDataString.c_str());
  // Send acknowledgement back to Processing
  Serial.print("E");
  Serial.println(edaData, 6);
  return edaData;
}
