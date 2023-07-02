const int GSR_PIN = A0;
unsigned long calibrationDuration = 5000;
unsigned long calibrationStartTime;
int baseline = 0;
int read_val = 0;

void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);

  // Start calibration
  // Serial.println("Calibration started. Please stay calm and do not move.");
  calibrate();
  // erial.println("Calibration complete.");
}

void loop() {
  // Read the input on analog pin A0
  int sensorValue = analogRead(GSR_PIN);

  // Send the current reading and baseline separated by a space
  // The Serial Plotter will plot these as two separate data streams.
  Serial.print(sensorValue);
  Serial.print(" ");
  Serial.println(baseline);

  // Delay to get a stable reading
  delay(100); // delay in milliseconds
}

void calibrate() {
  calibrationStartTime = millis();
  int readingCount = 0;
  unsigned long total = 0;

  while (millis() - calibrationStartTime < calibrationDuration) {
    read_val = analogRead(GSR_PIN);
    if (read_val > 0) {
      total += read_val;
      readingCount++;
    }
    delay(10);
  }

  baseline = total / readingCount;
}
