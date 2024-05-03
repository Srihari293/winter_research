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
float base_line = 75.0;

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
float threshold = 0.7;
float HR_baseline = 0;

int calibration = 0;
int calib_iterations = 10;
int calibrated = 0;
int statement = 0;

int s1_peaks = 0;
int s1_counter = 0;
long int s1_sum_HR = 0.0;

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
      //Serial.println(val);
      if (val == 'C') {
        Serial.println("Calibration starting");
        calibration = 1;
      }
      if (val == 'S') {
        statement++;
        Serial.print("Recording started for statement"); Serial.println(statement);
      }

      // Serial.print("Statement - ");
      // Serial.println(statement);

      if (calibration == 1) {
//        startCalibration();
        delay(1000);
        Serial.println("Calibration completed");
        calibration = 0;
        calibrated = 1;
        avg_HR  =  sum_HR / (float)i;
        avg_EDA =  sum_EDA / (float)j;
        HR_baseline = threshold * (max_HR - avg_HR) + avg_HR;
      }

      //val = Serial.read(); // Read data from Processing
      Serial.print("val: ");
      Serial.println(val);
      // Reading current EDA value for lie detection

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
//            if (curr_HR > max_HR) {
//              Serial.print("Max HR assigned: ");
//              Serial.println(curr_HR);
//              max_HR = curr_HR;
//              s3_peaks++;
//            }

            s3_sum_HR = s3_sum_HR + curr_HR;
//            Serial.print("S3 sum");
//            Serial.println(s3_sum_HR);
//            Serial.print("k: ");
//            Serial.println(k);
            s3_counter++;
          }
        }
        float avg = s3_sum_HR/s3_counter;
        Serial.println("avg");
        Serial.println(avg);
        delay(2000);
        if(avg > base_line){
          actuateNose();
          delay(2000);
        }
        s3_sum_HR = 0;
        s3_counter = 0;
        }
      }

//      else if (val == 'X')
//      {
//        //delay(100);
//        Serial.println("Statement 1 - recording");
//        for (int k = 0; k < 250; k++) {
//          //Serial.println("s1 loop");
//          delay(50);
//          val = Serial.read();
//          if (val == 'H') {            // HR data received
//            curr_HR = processHRData(); // Process HR data
//            if (curr_HR > max_HR) {
//              Serial.print("Max HR assigned: ");
//              Serial.println(curr_HR);
//              max_HR = curr_HR;
//              s1_peaks++;
//            }
//            s1_sum_HR = s1_sum_HR + curr_HR;
////            Serial.print("S sum");
////            Serial.println(s1_sum_HR);
////            Serial.print("k: ");
////            Serial.println(k);
//            s1_counter++;
//          }
//        }
//      Serial.println("Statement 1 - Completed");
//      }
//      else if (val == 'Y')
//      {
//        //delay(100);
//        Serial.println("Statement 2 - recording");
//        for (int k = 0; k < 250; k++) {
//          delay(50);
//          val = Serial.read();
//          if (val == 'H') {            // HR data received
//            curr_HR = processHRData(); // Process HR data
//            if (curr_HR > max_HR) {
//              Serial.print("Max HR assigned: ");
//              Serial.println(curr_HR);
//              max_HR = curr_HR;
//              s2_peaks++;
//            }
//
//            s2_sum_HR = s2_sum_HR + curr_HR;
////            Serial.print("S2 SUM");
////            Serial.println(s2_sum_HR);
////            Serial.print("k: ");
////            Serial.println(k);
//            s2_counter++;
//          }
//        }
//      Serial.println("Statement 2 - Completed");
//      }
//      else if (val == 'Z')
//      {
//        //delay(100);
//        Serial.println("Statement 3 - recording");
//        for (int k = 0; k < 250; k++) {
//          delay(50);
//          val = Serial.read();
//          //Serial.println("s3 loop");
//          if (val == 'H') {            // HR data received
//            curr_HR = processHRData(); // Process HR data
//            if (curr_HR > max_HR) {
//              Serial.print("Max HR assigned: ");
//              Serial.println(curr_HR);
//              max_HR = curr_HR;
//              s3_peaks++;
//            }
//
//            s3_sum_HR = s3_sum_HR + curr_HR;
////            Serial.print("S3 sum");
////            Serial.println(s3_sum_HR);
////            Serial.print("k: ");
////            Serial.println(k);
//            s3_counter++;
//          }
//        }
//      Serial.println("Statement 3 - Completed");  
//      }

//      else if (val == 'C')
//      {
//        Serial.println("Statements being saved\nCalculating metrics");
//        findMetrics();
//        hrComparison();
//        Serial.println("Calculations completed");
//      }

      // Comparing average HR to baseline

      // Actuation
//      actuation();

      //      if (calibrated == 1) {
      //        Serial.print("Min EDA: "); Serial.print(min_EDA, 6);
      //        Serial.print(" \t| Max EDA: "); Serial.println(max_EDA, 6);
      //        Serial.print(" Min HR: "); Serial.print(min_HR);
      //        Serial.print(" \t| Max HR: "); Serial.println(max_HR);
      //        Serial.print(" Average EDA: "); Serial.print(avg_EDA,6);
      //        Serial.print(" \t| Average HR : "); Serial.println(avg_HR);
      //        Serial.print(" Current EDA: "); Serial.print(curr_EDA,6);
      //        Serial.print(" \t| Current HR: "); Serial.println(curr_HR);
      //        Serial.println("-------------------------------------------------------");
      //
      //        // detectLieHR(HR_baseline);
      //        // delay(125);
      //      }
    }
  }


void establishContact()
{
  while (!handshakeDone)
  {
    if (Serial.available())
    {
      val = Serial.read();
      // Uncomment and use for debugging
      //Serial.print("VAL: ");
      //Serial.println(val);

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

  // Uncomment and use for Debugging - Send acknowledgement back to Processing
  //  Serial.print("EDA as string: "); Serial.println(edaDataString);
  //  Serial.print("E"); Serial.println(edaData, 6);

  return edaData;
}

int processHRData() {
  String HRDataString = Serial.readStringUntil('\n'); // Read HR data from Processing
  int HRData = atoi(HRDataString.c_str());            // Convert the string to a float

  // Uncomment and use for Debugging - Send acknowledgement back to Processing
  // Serial.print("HR as string: "); Serial.println(HRDataString);
  // Serial.print("H"); Serial.println(HRData);

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

  // if not blowing, vent to reset the nose state
  else if (state != VENTING && digitalRead(switch_)) {
    Serial.println("Resetting");
    switchOnPump(1, 100);
    blow();

    switchOffPumps();
    vent();
    state = VENTING;
  }
}
