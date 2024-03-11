#include <Wire.h>
#include <Servo.h>


const int mpuAddress = 0x68; // 0b1101000 from datasheet Section 9.2
const int greLED = 11;
const int redLED = 2;
const int buzzerPin = 4;

float AcX, AcY, AcZ, GyX, GyY, GyZ;

float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;

float roll, pitch, yaw;

float sensScalar = 16384.0; // from datasheet 
float gyroScalar = 131.0; // from datasheet, section 6.1 

float elapsedTime, currentTime, previousTime;

int servoAngle = 90;


Servo myServo;

void setup() {
  Serial.begin(9600);

  Wire.begin(); 
  Wire.beginTransmission(mpuAddress); 

  // Register at the address 0x6B is todo with power management
  // Writing 0 to this register forces a reset, without having to cut power
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  

  delay(20);

  pinMode(greLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  myServo.attach(6);
}
void loop() {
  // === Read acceleromter data === //
  Wire.beginTransmission(mpuAddress);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(mpuAddress, 14, true); // Read 6 registers total, each axis value is stored in 2 registers

  AcX = (Wire.read() << 8 | Wire.read()) / sensScalar; 
  AcY = (Wire.read() << 8 | Wire.read()) / sensScalar; 
  AcZ = (Wire.read() << 8 | Wire.read()) / sensScalar;

  (Wire.read() << 8 | Wire.read()); // fetch then Disregard, this is faster then doing two transmissions of 6 registers each

  GyX = (Wire.read() << 8 | Wire.read()) / gyroScalar; 
  GyY = (Wire.read() << 8 | Wire.read()) / gyroScalar;
  GyZ = (Wire.read() << 8 | Wire.read()) / gyroScalar;


  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  
  
  accAngleX = (atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180 / PI);
  accAngleY = (atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180 / PI);

  // The raw gyroscope data is in degrees per second, multiplying by time gets the acutual degree, lots of data innacuracy occurs here.
  gyroAngleX += (GyX * elapsedTime); // deg/s * s = deg
  gyroAngleY += (GyY * elapsedTime);

  yaw =  yaw + GyZ * elapsedTime;


  // Complementary filter - combine acceleromter and gyro angle values
  gyroAngleX = 0.96 * gyroAngleX + 0.04 * accAngleX;
  gyroAngleY = 0.96 * gyroAngleY + 0.04 * accAngleY;

  roll = gyroAngleX;
  pitch = gyroAngleY;
  

  Serial.print(" Roll: ");   Serial.print(roll);
  Serial.print(" | Pitch: ");   Serial.print(pitch);
  Serial.print(" | Yaw: ");   Serial.print(yaw);
  Serial.println();


  servoAngle = map(yaw, 45, -45, 180, 0);

  if (pitch <= -50) { // Overflow Mode, move to stop postion, red lights, and speaker on
    servoAngle = 180;
    digitalWrite(buzzerPin, HIGH);
  }
  else {
    digitalWrite(buzzerPin, LOW);
  }

  myServo.write(servoAngle);

  // lighting code, if valve is closed, red light on, green light on otherwise
  if (servoAngle >= 180) {
    digitalWrite(greLED, LOW);
    digitalWrite(redLED, HIGH);
  }
  else {
    digitalWrite(redLED, LOW);
    digitalWrite(greLED, HIGH);
  }


  delay(20);
}
