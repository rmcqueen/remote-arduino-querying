#include <ArduinoUnit.h>
#include <DHT.h>
#include <SPI.h>
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

test(pinNumberConfig)
{
  assertEqual(2, DHTPIN);
}

test(sensorTypeConfig)
{
  assertEqual(DHT22, DHTTYPE);  
}

test(temperatureReading)
{
  int temperatureReading = dht.readTemperature('c');
  assertEqual(false, isnan(temperatureReading));
}

void setup()
{
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
  Test::run();
}

