#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

//number of temperature sensors PREDEFINED
const int numDS18B20 = 3;
float tempC[numDS18B20];

void setup(void)
{
  sensors.begin();  // Start up the library
  Serial.begin(9600);
  
  // locate devices on the bus IF deviceCount
  //Serial.print("Locating devices...");
  //Serial.print("Found ");
  //deviceCount = sensors.getDeviceCount();
  //Serial.print(deviceCount, DEC);
  //Serial.println(" devices.");
  //Serial.println("");
}

void loop(void)
{ 
  // Send command to all the sensors for temperature conversion
  sensors.requestTemperatures(); 
  
  // Display temperature from each sensor
  for (int i = 0;  i < numDS18B20;  i++)
  {
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(" : ");
    tempC[i] = sensors.getTempCByIndex(i);
    Serial.print(tempC[i]);
    Serial.print((char)176);//shows degrees character
    Serial.print("C");
  }
  
  Serial.println("");
  delay(1000);
}
