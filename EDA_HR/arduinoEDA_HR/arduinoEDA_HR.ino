char val; // Data received from the serial port
int ledPin = 11; // Set the pin to digital I/O 13
bool handshakeDone = false; // Flag to indicate if the handshake is done
float min_EDA = 200;
float max_EDA = 0;
float curr_EDA = 0;

int min_HR = 200;
int max_HR = 0;
int curr_HR = 0;

int i = 0;
int j = 0;

void setup() {
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
  Serial.begin(9600); // Start serial communication at 9600 bps
}

void loop() {
  if (!handshakeDone) {
    establishContact(); // Perform handshake with Processing
   } 

   else {
    if (Serial.available()) {
      val = Serial.read(); // Read data from Processing

      // Heart rate processing
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

        Serial.print("Current HR: "); Serial.print(curr_HR);
        Serial.print(" | Max HR: "); Serial.print(max_HR); 
        Serial.print("| Min HR: "); Serial.println(min_HR);
        i++;
      }

      // EDA processing
      if (val == 'E') {            // EDA data received
        curr_EDA = processEDAData(); // Process EDA data
        if (curr_EDA > max_EDA) {
          Serial.print("Max EDA assigned: ");
          Serial.println(curr_EDA,6);
          max_EDA = curr_EDA;
        }
        
        else if (curr_EDA < min_EDA) {
          Serial.print("Min EDA assigned: ");
          Serial.println(curr_EDA,6);
          min_EDA = curr_EDA;
        }

        Serial.print("Current EDA: "); Serial.print(curr_EDA,6);
        Serial.print(" | Max EDA: "); Serial.print(max_EDA,6); 
        Serial.print("| Min EDA: "); Serial.println(min_EDA,6);
        Serial.print("++++++++++++++++++++++++++++++++++++++++");
        j++;
      }
      
      //Serial.print("counter: ");
      //Serial.println(i);
      if (i >= 500 && j >= 500) {
        Serial.print("Min EDA: "); Serial.print(min_EDA,6);
        Serial.print(" | Max EDA: "); Serial.print(max_EDA,6);
        Serial.print(" | Min HR: "); Serial.print(min_HR);
        Serial.print(" | Max HR: "); Serial.println(max_HR);
      }
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
        Serial.println("here");
        handshakeDone = true; // Handshake is done
        digitalWrite(ledPin, HIGH); // Turn on the LED to indicate successful handshake
        Serial.println("here2");
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
   //edaDataString=edaDataString.remove(0,1);
  // Convert the string to a float
  float edaData = atof(edaDataString.c_str());

  // Send acknowledgement back to Processing
  Serial.print("E");
  Serial.println(edaData,6);
  Serial.println("----------------------");
  return edaData;
}

int processHRData() {
  // Read HR data from Processing
   String HRDataString = Serial.readStringUntil('\n');
   Serial.print("HR as string: ");
   Serial.println(HRDataString);
   //edaDataString=edaDataString.remove(0,1);
  // Convert the string to a float
  int HRData = atoi(HRDataString.c_str());

  // Send acknowledgement back to Processing
  Serial.print("H");
  Serial.println(HRData);
  Serial.println("----------------------");
  return HRData;
}
