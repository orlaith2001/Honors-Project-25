/*
 * MPU6050 Angle Test
 * Prints tilt angles on X and Y, and rotation angle on Z.
 * License: MIT
 */

#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial on boards like Mega
  Serial.println("Setup started");

  Wire.begin();
  delay(100); // Allow I2C bus to stabilize

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);

  // Retry loop if MPU6050 is not responding
  while (status != 0) {
    Serial.println("MPU6050 init failed. Retrying...");
    delay(1000);
    status = mpu.begin();
  }

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // Uncomment this if your MPU6050 is mounted upside-down
  // mpu.upsideDownMounting = true;
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Offsets calculated. Setup complete!\n");
}

void loop() {
  mpu.update();

  if ((millis() - timer) > 2000) { // Print every 2 seconds
    Serial.print("X : ");
    Serial.print(mpu.getAngleX(), 1);
    Serial.print("\tY : ");
    Serial.print(mpu.getAngleY(), 1);
    Serial.print("\tZ : ");
    Serial.println(mpu.getAngleZ(), 1);
    timer = millis();
  }
}

