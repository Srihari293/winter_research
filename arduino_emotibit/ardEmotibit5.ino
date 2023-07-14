char val; // Data received from the serial port
int ledPin = 11; // Set the pin to digital I/O 13
bool handshakeDone = false; // Flag to indicate if the handshake is done

void setup() {
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
  Serial.begin(9600); // Start serial communication at 9600 bps
}

void loop() {
  if (!handshakeDone) {
    establishContact(); // Perform handshake with Processing
  } else {
    if (Serial.available()) {
      val = Serial.read(); // Read data from Processing

      if (val == 'E') { // EDA data received
        processEDAData(); // Process EDA data
      } else if (val == 'P') { // PPGIR data received
        processPPGIRData(); // Process PPGIR data
      }
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

void processEDAData() {
  // Read EDA data from Processing
  long int edaData = Serial.parseInt();

  // Process EDA data as needed
  // ...

  // Send acknowledgement back to Processing
  Serial.println("E" + String(edaData));
}

void processPPGIRData() {
  // Read PPGIR data from Processing
  long int ppgirData = Serial.parseInt();

  // Process PPGIR data as needed
  // ...

  // Send acknowledgement back to Processing
  Serial.println("P" + String(ppgirData));
}
