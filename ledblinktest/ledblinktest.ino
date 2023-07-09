#define LED_PIN 13 // Internal LED pin number

void setup() {
  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
}

void loop() {
  digitalWrite(LED_PIN, HIGH); // Turn on the LED
  delay(1000); // Wait for 1 second
  digitalWrite(LED_PIN, LOW); // Turn off the LED
  delay(1000); // Wait for 1 second
}
