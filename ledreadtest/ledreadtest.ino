void setup() {
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  for (int pin = 0; pin < 19; pin++) { // Iterate through all pins (0 to 19)
    int value = digitalRead(pin); // Read the digital value of the pin

    Serial.print("Pin ");
    Serial.print(pin);
    Serial.print(": ");
    Serial.println(value);

    delay(100); // Delay for a short period between reading each pin
  }
}

// a - 0,2,4,15
