#include <EmotiBit.h>

EmotiBit emotibit;

void setup() {
  // Begin serial communication
  Serial.begin(115200);
  while (!Serial);

  // Initialize EmotiBit
  emotibit.setup();
}

void loop() {
  if (emotibit.update()) {
    // Read EDA
    float eda = emotibit.eda.getMeanEDA();
    Serial.print("EDA: ");
    Serial.print(eda);
    Serial.print(" uS, ");

    // Read PPG (Heart Rate)
    float ppg = emotibit.ppg.getHeartRate();
    Serial.print("PPG: ");
    Serial.print(ppg);
    Serial.print(" BPM, ");

    // Read IMU data
    float ax, ay, az, gx, gy, gz, mx, my, mz;
    emotibit.imu.getAccel(ax, ay, az);
    emotibit.imu.getGyro(gx, gy, gz);
    emotibit.imu.getMag(mx, my, mz);
    Serial.print("Accel: ");
    Serial.print(ax);
    Serial.print(",");
    Serial.print(ay);
    Serial.print(",");
    Serial.print(az);
    Serial.print(" m/s^2, ");

    // Read Temperature
    float temperature = emotibit.temperature.getTemperatureC();
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.println(" C");
  }
}
