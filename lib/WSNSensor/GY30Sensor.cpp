#include "GY30Sensor.h"


void GY30Sensor::setup()
{
  Wire.beginTransmission(I2C_ADDRESS_GY_30);
  Wire.write(0x10); // 1 [lux] aufloesung
  Wire.endTransmission();
}

WSNSensorValue GY30Sensor::value()
{
  float valf=0;
  byte buffData[2];
  WSNSensorValue sensorValue(0, "Luminosity", "lx");
  byte i=0;
  Wire.beginTransmission(I2C_ADDRESS_GY_30);
  Wire.requestFrom(I2C_ADDRESS_GY_30, 2);
  while(Wire.available()){
    buffData[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();

  if(i != 2){
    // delay(500);
    return sensorValue;
  }

  valf=((buffData[0]<<8)|buffData[1])/1.2;

  if( valf < 0 ) {
    sensorValue.value =  65535;
  }
  else{
    sensorValue.value = (int)valf;
  }

  return sensorValue;
}
