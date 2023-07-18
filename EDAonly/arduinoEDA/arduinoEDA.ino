char val; // Data received from the serial port
int ledPin = 11; // Set the pin to digital I/O 13
bool handshakeDone = false; // Flag to indicate if the handshake is done
float min_EDA = 10;
float max_EDA = 0;
float curr_EDA = 0;
int i = 0;
int change_min=0;
int change_max=0;

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

      if (val == 'E') { // EDA data received
        curr_EDA = processEDAData(); // Process EDA data
        if (curr_EDA > max_EDA) {
          Serial.print("Max assigned");
          Serial.println(curr_EDA,6);
          max_EDA = curr_EDA;
          change_max++;
        }
        else if (curr_EDA < min_EDA) {
          Serial.print("Min assigned");
          Serial.println(curr_EDA,6);
          min_EDA = curr_EDA;
          change_min++;
        }

        Serial.print("CURR: ");
        Serial.println(curr_EDA,6);
        Serial.print("MAX: ");
        Serial.println(max_EDA,6); 
        Serial.print("MIN: "); 
        Serial.println(min_EDA,6);
        i++;
      }
      //Serial.print("counter: ");
      //Serial.println(i);
      if (i >= 500) {
        Serial.print("Min EDA: ");
        Serial.println(min_EDA,6);
        Serial.print("Max EDA: ");
        Serial.println(max_EDA,6);
        Serial.println(change_min);
        Serial.println(change_max);
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
   //edaDataString=edaDataString.remove(0,1);
  // Convert the string to a float
  float edaData = atof(edaDataString.c_str());

  // Send acknowledgement back to Processing

  Serial.print("E");
  Serial.println(edaData,6);
  Serial.println("----------------------");
  return edaData;
}
