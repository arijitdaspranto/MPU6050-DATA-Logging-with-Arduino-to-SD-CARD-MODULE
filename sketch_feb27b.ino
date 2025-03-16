#include <SPI.h>
#include <SD.h>
#include <Wire.h>

const int MPU_addr = 0x68; // I2C address of the MPU-6050
const int chipSelect = 10; // Chip select pin for SD card module

File dataFile;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    while (1); // Stop execution if SD card fails
  }

  dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time,AccX,AccY,AccZ,GyroX,GyroY,GyroZ"); // Header row
    dataFile.close();
    Serial.println("SD card initialized and file created.");
  } else {
    Serial.println("Error creating datalog.csv");
  }
}

void loop() {
  int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(millis()); // Time in milliseconds
    dataFile.print(",");
    dataFile.print(AcX);
    dataFile.print(",");
    dataFile.print(AcY);
    dataFile.print(",");
    dataFile.print(AcZ);
    dataFile.print(",");
    dataFile.print(GyX);
    dataFile.print(",");
    dataFile.print(GyY);
    dataFile.print(",");
    dataFile.println(GyZ);
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.csv");
  }

  delay(100); // Adjust delay as needed
}