// Libraries for DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
// Libraries for DHT11/22
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Sensor type for the humidity sensor
#define DHTTYPE DHT11   // DHT 11
// Data wire bus for DS18B20 in digital pin 2
#define ONE_WIRE_BUS 2
// Humidity sensor DHT INSIDE (pin 3) and OUTSIDE (pin 4) the beehive
#define DHT_IN_PIN 3
#define DHT_OUT_PIN 4

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to DallasTemperature library for DS18B20
DallasTemperature sensors(&oneWire);


// Number of temperature/humidity sensors PREDEFINED
const int numDS18B20 = 5;
const int numDHT = 2;
float tempC[numDS18B20]; //12 significant bits
DeviceAddress address_T1 = { 0x28, 0x28, 0xCF, 0xE0, 0x03, 0x00, 0x00, 0xE9 };
DeviceAddress address_T1 = { 0x28, 0x28, 0xCF, 0xE0, 0x03, 0x00, 0x00, 0xE9 };
DeviceAddress address_T1 = { 0x28, 0x28, 0xCF, 0xE0, 0x03, 0x00, 0x00, 0xE9 };
DeviceAddress address_T1 = { 0x28, 0x28, 0xCF, 0xE0, 0x03, 0x00, 0x00, 0xE9 };
DeviceAddress address_T1 = { 0x28, 0x28, 0xCF, 0xE0, 0x03, 0x00, 0x00, 0xE9 };

float temperature[numDHT];
float humidity[numDHT];

// Initialize the DHT sensors INSIDE and OUTSIDE the beehive
DHT_Unified dht_in(DHT_IN_PIN, DHTTYPE);
DHT_Unified dht_out(DHT_OUT_PIN, DHTTYPE);


void setup(void)
{
  // Start up the library for DS18B20 and for DHT
  sensors.begin();
  dht.begin();
  Serial.begin(9600);
  
}

void loop(void)
{ 
  // Send command to all DS18B20 for temperature conversion
  sensors.requestTemperatures();
  
  // Get temperature from each sensor and display it in order to test/debug
  for (int i = 0;  i < numDS18B20;  i++)
  {
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(" : ");
    //RICONOSCIMENTO SENSORE
    tempC[i] = sensors.getTempCByIndex(i);
    Serial.print(tempC[i]);
    Serial.print("C ");
  }
  
  Serial.println("");
  
  // Returns true if a measurement is available
  if(dht_sensor_in.measure(&temperature[0], &humidity[0])){
    Serial.print( "T = " );
    Serial.print( temperature[0], 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity[0], 1 );
    Serial.println( "%" );
  }
  //else{
    //TRY FOR SOME TIME AND IF IT DOESN'T RESPOND THAN ALWAYS RETURN AN ERROR VALUE AND CALL IT NO MORE
  //}

  //NEED TO MODIFY THE LIBRARIES BECAUSE THEY WORK ONLY IF A MEASUREMENT IS MADE EVERY 4s
  if(dht_sensor_out.measure(&temperature[1], &humidity[1])){
    Serial.print( "T = " );
    Serial.print( temperature[1], 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity[1], 1 );
    Serial.println( "%" );
  }
    
}
