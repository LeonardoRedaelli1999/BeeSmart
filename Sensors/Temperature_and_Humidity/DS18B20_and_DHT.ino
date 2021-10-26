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

// Addresses of DS18B20 sensors retrived with the Sensor_Identifier.ino program. This procedure can be AUTOMATIZED for a large scale production 
DeviceAddress address_T1 = {0x28, 0x96, 0x25, 0x46, 0x5F, 0x20, 0x01, 0x65}; // T1=CLUSTER of bees
DeviceAddress address_T2 = {0x28, 0x3E, 0xA8, 0x93, 0x63, 0x20, 0x01, 0x5A}; // T2=LEFT or EST STOCKS
DeviceAddress address_T3 = {0x28, 0xE2, 0x8A, 0x7A, 0x62, 0x20, 0x01, 0xE9}; // T3=RIGHT or OVEST STOCKS
DeviceAddress address_T4 = {0x28, 0x65, 0x7B, 0x5C, 0x5F, 0x20, 0x01, 0x5A}; // T4=CLUSTER of bees (safety)
DeviceAddress address_T5 = {0x28, 0x49, 0x7C, 0x51, 0x5F, 0x20, 0x01, 0x5D}; // T5=OUTSIDE

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
