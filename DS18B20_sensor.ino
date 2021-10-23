// Libraries for DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
// Libraries for DHT11/22
#include <dht_nonblocking.h>

// Sensor type for the humidity sensor
#define DHT_SENSOR_TYPE DHT_TYPE_11
// Data wire bus for DS18B20 in digital pin 2
#define ONE_WIRE_BUS 2
// Humidity sensor pin 3
#define DHT_SENSOR_PIN 3

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  
// Pass oneWire reference to DallasTemperature library for DS18B20
DallasTemperature sensors(&oneWire);


//number of temperature/humidity sensors PREDEFINED
const int numDS18B20 = 5;
const int numDHT = 2;
float tempC[numDS18B20];
float temperature[numDHT];
float humidity[numDHT];

//need to implement the sensors as an array
DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void setup(void)
{
  sensors.begin();  // Start up the library for DS18B20
  Serial.begin(9600);
  
}

void loop(void)
{ 
  // Send command to all DS18B20 for temperature conversion
  sensors.requestTemperatures();
  
  // Display temperature from each sensor
  for (int i = 0;  i < numDS18B20;  i++)
  {
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(" : ");
    tempC[i] = sensors.getTempCByIndex(i);
    Serial.print(tempC[i]);
    Serial.print("C");
  }
  
  Serial.println("");
  
  if(dht_sensor.measure(&temperature, &humidity)){
    Serial.print( "T = " );
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );
    
  delay(1000);
}
