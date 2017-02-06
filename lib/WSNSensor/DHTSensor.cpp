#include "DHTSensor.h"

DHT DHTSensor::_dht(DHTPIN, DHTTYPE);

void DHTSensor::setup(){
  DHTSensor::_dht.begin();
}

WSNSensorValue DHTSensor::value(){

//TODO
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = DHTSensor::_dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = DHTSensor::_dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = DHTSensor::_dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("[SENSOR ERROR] Failed to read from DHT sensor!");
    return WSNSensorValue(0, "Temperature", "*C");
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = DHTSensor::_dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = DHTSensor::_dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  return WSNSensorValue(hic, "Temperature", "*C");
// Read more: http://www.cnx-software.com/2016/03/22/getting-started-with-wemos-d1-mini-esp8266-board-dht-relay-shields/#ixzz4ABRyqUZu
}

WSNSensorValue DHTSensor::temperature(){
  // Read temperature as Celsius (the default)
  float t = DHTSensor::_dht.readTemperature();

  if (isnan(t)) {
    Serial.println("[SENSOR ERROR] Failed to read from DHT sensor!");
    return WSNSensorValue(0, "Temperature", "*C");
  }

  return WSNSensorValue(t, "Temperature", "*C");
}

WSNSensorValue DHTSensor::humidity(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = DHTSensor::_dht.readHumidity();
  if (isnan(h)) {
    Serial.println("[SENSOR ERROR] Failed to read from DHT sensor!");
    return WSNSensorValue(0, "Humidity", "%");
  }

  return WSNSensorValue(h, "Humidity", "%");

}

WSNSensorValue DHTSensor::heatIndex(){
  float h = DHTSensor::humidity().value;
  float t = DHTSensor::temperature().value;
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = DHTSensor::_dht.computeHeatIndex(t, h, false);

  return WSNSensorValue(hic, "Heat Index", "*C");
}
