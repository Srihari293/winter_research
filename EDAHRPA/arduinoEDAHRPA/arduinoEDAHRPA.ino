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
int calib_iterations = 200;
int calibrated = 0;
int statement = 0;
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
      Serial.print("Val");
      Serial.println(val);
      if (val == 'C') {
        Serial.println("Calibration starting");
        calibration = 1;
      }
      if (val =='S'){
        statement++;
        Serial.print("Recording started for statement"); Serial.println(statement);
        }
      
      Serial.print("Statement - ");
      Serial.println(statement);

      if (calibration == 1) {
        startCalibration();
        delay(1000);
        Serial.println("Calibration completed");
        calibration = 0;
        calibrated = 1;
        avg_HR  =  sum_HR / (float)i;
        avg_EDA =  sum_EDA / (float)j;
        HR_baseline = threshold * (max_HR - avg_HR) + avg_HR;
      }


      // Reading current EDA value for lie detection
      val=Serial.read();
      if (val=='E'){
          curr_EDA = processEDAData();
      }
      else if(val=='H'){
         curr_HR = processHRData();
      }
     
    
      

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
  Serial.print("EDA as string: "); Serial.println(edaDataString);
  Serial.print("E"); Serial.println(edaData,6);

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

    for (c = 0; c < 450; c++)
    {
      // switch on pumps to 50% power
      switchOnPump(2, 100);
      switchOffPump(1);
      // Serial.println(c);
      blow();
      state = BLOWING;
    }
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

void startCalibration() {
  for (int calib = 0; calib < calib_iterations; calib++) {
    val = Serial.read();

    if (val == 'H') {            // HR data received
      curr_HR = processHRData(); // Process HR data
      if (curr_HR > max_HR) {
        Serial.print("Max HR assigned: ");
        Serial.println(curr_HR);
        max_HR = curr_HR;
      }

      else if (curr_HR < min_HR) {
        Serial.print("Min HR assigned: ");
        Serial.println(curr_HR);
        min_HR = curr_HR;
      }

      Serial.print("Current HR : "); Serial.print(curr_HR);
      Serial.print("\t | Max HR : "); Serial.print(max_HR);
      Serial.print("\t | Min HR : "); Serial.println(min_HR);
      sum_HR = sum_HR + curr_HR;
      i++;
    }

    // EDA processing
    if (val == 'E') {            // EDA data received
      curr_EDA = processEDAData(); // Process EDA data
      if (curr_EDA > max_EDA) {
        Serial.print("Max EDA assigned: ");
        Serial.println(curr_EDA, 6);
        max_EDA = curr_EDA;
      }

      else if (curr_EDA < min_EDA) {
        Serial.print("Min EDA assigned: ");
        Serial.println(curr_EDA, 6);
        min_EDA = curr_EDA;
      }

      Serial.print("Current EDA: "); Serial.print(curr_EDA, 6);
      Serial.print(" | Max EDA: "); Serial.print(max_EDA, 6);
      Serial.print(" | Min EDA: "); Serial.println(min_EDA, 6);
      Serial.println("--------------------------------------------------------------");
      sum_EDA = sum_EDA + curr_EDA;
      j++;
    }
    delay(100);
    // Serial.print("Calibration iteration = "); Serial.println(calib);
  }
}
void detectLieHR(float HR_baseline) {
  if (curr_HR > HR_baseline)
  {
    Serial.println("Lie detected!");
    Serial.print("Current HR: ");
    Serial.print(curr_HR);
    Serial.print("\t HR baseline: ");
    Serial.println(HR_baseline);
    actuateNose();
  }
}
