
#include<Wire.h>
// I2C address of the GY-521
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
void setup(){
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    // PWR_MGMT_1 register
    Wire.write(0x6B);
    // set to zero (wakes up the GY-521)
    Wire.write(0);
    Wire.endTransmission(true);
    Serial.begin(9600);
}
void loop(){
      Wire.beginTransmission(MPU_addr);
      // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.write(0x3B);
      Wire.endTransmission(false);
      // request a total of 14 registers
      Wire.requestFrom(MPU_addr,14,true);
      // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
      AcX=Wire.read()<<8|Wire.read();
      // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
      AcY=Wire.read()<<8|Wire.read();
      // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

      AcZ=Wire.read()<<8|Wire.read();
      // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
      Tmp=Wire.read()<<8|Wire.read();
      // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
      GyX=Wire.read()<<8|Wire.read();
      // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
      GyY=Wire.read()<<8|Wire.read();
      // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
      GyZ=Wire.read()<<8|Wire.read();
      Serial.print("AcX = "); Serial.print(AcX);
      Serial.print(" | AcY = "); Serial.print(AcY);
      Serial.print(" | AcZ = "); Serial.print(AcZ);
      //equation for temperature in degrees C from datasheet
      Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);
      Serial.print(" | GyX = "); Serial.print(GyX);
      Serial.print(" | GyY = "); Serial.print(GyY);
      Serial.print(" | GyZ = "); Serial.println(GyZ);
      delay(333);
}