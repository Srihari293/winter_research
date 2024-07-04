#include "programmable_air.h"
#include <Adafruit_NeoPixel.h>

#define DEBUG 1

int state = UN_KNOWN;

int atmospheric_pressure = 508; // should be around 508
int switch_ = 11;

char val; // Data received from the serial port
// int ledPin = 11; // Set the pin to digital I/O 13
bool handshakeDone = false; // Flag to indicate if the handshake is done
float min_EDA = 200;
float max_EDA = 0;
float curr_EDA = 0;
//float base_line = 75.0;

int min_HR = 200;
int max_HR = 0;
int curr_HR = 0;

int c = 0;
long int i = 0;
long int j = 0;
float sum_EDA = 0;
long int sum_HR = 0;
float avg_EDA = 0;
float avg_HR = 0;
float threshold = 1.1;
float HR_baseline = 0.0;

int calibration = 1;
int calib_iterations = 10;
int calibrated = 0;
int statement = 0;

int s1_peaks = 0;
int s1_counter = 0;
long int s1_sum_HR = 0.0;
int counter = 0;

int s2_peaks = 0;
int s2_counter = 0;
long int s2_sum_HR = 0.0;

int s3_peaks = 0;
int s3_counter = 0;
long int s3_sum_HR = 0.0;

float s1_avg_HR;
float s2_avg_HR;
float s3_avg_HR;

int lie_flag = 0;
void setup() {
  initializePins();
  //  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
  pinMode(switch_, INPUT_PULLUP);
  Serial.begin(9600); // Start serial communication at 9600 bps
}

void loop() {
  if (!handshakeDone) {
    establishContact(); // Perform handshake with Processing
  }

  else {
    if (Serial.available()) {
      val = Serial.read(); // Read data from Processing
      Serial.print("val: ");
      Serial.println(val);

      if (calibration == 1) {
        delay(1000);
        calibration = 0;
        calibrated = 1;

        Serial.println("Prelim calibration");
        for (int i = 0; i < 500; i++) {
          delay(50);
          val = Serial.read();
          if (val == 'H') {            // HR data received
            curr_HR = processHRData(); // Process HR data
            sum_HR = sum_HR + curr_HR;
            counter++;
          }
        }
        
        HR_baseline = sum_HR/counter;
        HR_baseline = threshold * HR_baseline;
        Serial.print("Calibrated value");
        Serial.println(HR_baseline);
        Serial.println("Calibration completed for the first time");
      }

      if (val == 'E') {
        curr_EDA = processEDAData();
      }

      else if (val == 'H') {
        curr_HR = processHRData();
        Serial.println("Statement 3 - recording");
        for (int k = 0; k < 300; k++) {
          delay(50);
          val = Serial.read();
          //Serial.println("s3 loop");
          if (val == 'H') {            // HR data received
            curr_HR = processHRData(); // Process HR data
            s3_sum_HR = s3_sum_HR + curr_HR;
            s3_counter++;
          }
        }
        float avg = s3_sum_HR/s3_counter;
        Serial.println("avg");
        Serial.println(avg);
        delay(2000);
        if(avg > HR_baseline){
          actuateNose();
          delay(2000);
        }
        s3_sum_HR = 0;
        s3_counter = 0;
        }
      }
    }
  }


void establishContact()
{
  while (!handshakeDone)
  {
    if (Serial.available())
    {
      val = Serial.read();
      if (val == 'A')
      {
        Serial.println("A");  // Respond with 'A' to confirm contact
        handshakeDone = true; // Handshake is done
      }
    }
    else
    {
      Serial.println("B");    // Send 'A' contact
      Serial.println("Requesting contact");
      delay(300);
    }
  }
}

float processEDAData() {
  String edaDataString = Serial.readStringUntil('\n'); // Read EDA data from Processing
  float edaData = atof(edaDataString.c_str()); // Convert the string to a float
  return edaData;
}

int processHRData() {
  String HRDataString = Serial.readStringUntil('\n'); // Read HR data from Processing
  int HRData = atoi(HRDataString.c_str());            // Convert the string to a float
  return HRData;
}

void actuateNose() {
  if (state != BLOWING) {
    // Serial.println("Blowing");

    for (c = 0; c < 100; c++)
    {
      // switch on pumps to 50% power
      switchOnPump(2, 100);
      switchOffPump(1);
      // Serial.println(c);
      blow();
      Serial.println("Blowing");
      Serial.println(c);
      state = BLOWING;
    }
    Serial.println("Done blowing");
    state = UN_KNOWN;
    switchOffPump(2);
  }
  
  else if (state != VENTING && digitalRead(switch_)) {
    Serial.println("Resetting");
    switchOnPump(1, 100);
    blow();

    switchOffPumps();
    vent();
    state = VENTING;
  }
}
