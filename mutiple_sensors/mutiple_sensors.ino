#include <EmotiBit.h>

EmotiBit emotibit;

// Baselines for sensors
float baselinePPG = 0;
float baselineGSR = 0;

// Threshold for what is considered a "spike" (this value needs calibration)
float spikeThreshold = 10.0;

// Counters for alerts
int ppgAlerts = 0;
int gsrAlerts = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial); // Wait for Serial monitor to open
  Serial.println("Emotibit Setup");
  emotibit.setup();
  Serial.println("Emotibit Setup completed");
  Serial.println("Calibration in progress.");
  // Calibrate baselines
  calibrateBaseline();
  Serial.println("Calibration complete.");
}

void loop() {
  // Read and record the timestamp
  unsigned long timestamp = millis();
  
  // Read sensor data
  float ppgValue = readPPG();
  float gsrValue = readGSR();
  
  // Output the data to the Serial Monitor
  Serial.print("Timestamp: "); Serial.print(timestamp);
  Serial.print(" | PPG: "); Serial.print(ppgValue);
  Serial.print(" | GSR: "); Serial.println(gsrValue);
  
  // Check for spikes
  // checkForSpike(ppgValue, baselinePPG, ppgAlerts, "PPG");
  // checkForSpike(gsrValue, baselineGSR, gsrAlerts, "GSR");
  
  // Delay before the next reading
  delay(1000);
}

float readPPG() {
  // Placeholder - use actual code to read PPG value from EmotiBit
  Serial.println("Attempting to call read sensor");
  emotibit.readSensors();
  return 1.0;
}

float readGSR() {
  // Placeholder - use actual code to read GSR value from EmotiBit
  return 0.0;
}

void checkForSpike(float value, float baseline, int &alertCounter, const char* sensorName) {
  if (abs(value - baseline) > spikeThreshold) {
    alertCounter++;
    Serial.print("Alert for "); Serial.print(sensorName); Serial.print(". Count: "); Serial.println(alertCounter);
  }
}

void calibrateBaseline() {
  const int calibrationPeriod = 10000; // 10 seconds in milliseconds
  const int sampleInterval = 100; // 100ms
  
  int numSamples = calibrationPeriod / sampleInterval;
  float sumPPG = 0, sumGSR = 0;

  for (int i = 0; i < numSamples; i++) {
    sumPPG += readPPG();
    sumGSR += readGSR();
    delay(sampleInterval);
  }

  baselinePPG = sumPPG / numSamples;
  baselineGSR = sumGSR / numSamples;
}
