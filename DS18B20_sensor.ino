// Libraries for DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
// Libraries for DHT11/22
#include <dht_nonblocking.h>

// Sensor type for the humidity sensor
#define DHT_SENSOR_TYPE DHT_TYPE_11
// Data wire bus for DS18B20 in digital pin 2
#define ONE_WIRE_BUS 2
// Humidity sensor DHT INSIDE (pin 3) and OUTSIDE (pin 4) the beehive
#define DHT_SENSOR_IN_PIN 3
#define DHT_SENSOR_OUT_PIN 4

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  
// Pass oneWire reference to DallasTemperature library for DS18B20
DallasTemperature sensors(&oneWire);


// Number of temperature/humidity sensors PREDEFINED
const int numDS18B20 = 5;
const int numDHT = 2;
float tempC[numDS18B20];
float temperature[numDHT];
float humidity[numDHT];

// Initialize the DHT sensors
DHT_nonblocking dht_sensor_in(DHT_SENSOR_IN_PIN, DHT_SENSOR_TYPE);
DHT_nonblocking dht_sensor_out(DHT_SENSOR_OUT_PIN, DHT_SENSOR_TYPE);


void setup(void)
{
  // Start up the library for DS18B20
  sensors.begin();
  
  Serial.begin(9600);
  
}

void loop(void)
{ 
  // Send command to all DS18B20 for temperature conversion
  sensors.requestTemperatures();
  
  // Get temperature from each sensor and display it in order to test/debug
  for (int i = 0;  i < numDS18B20;  i++)
  {
    //Serial.print("Sensor ");
    //Serial.print(i+1);
    //Serial.print(" : ");
    tempC[i] = sensors.getTempCByIndex(i);
    //Serial.print(tempC[i]);
    //Serial.print("C");
  }
  
  Serial.println("");
  
  // Returns true if a measurement is available
  if(dht_sensor_in.measure(&temperature, &humidity)){
    Serial.print( "T = " );
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );
  }
  //else{
    //TRY FOR SOME TIME AND IF IT DOESN'T RESPOND THAN ALWAYS RETURN AN ERROR VALUE AND CALL IT NO MORE
  //}
  
  //NOT SURE WHETHER IT'S JUST A +1 TO GIVE HIM THE POINTER TO THE NEXT CELL OF THE ARRAY
  if(dht_sensor_out.measure(&temperature+1, &humidity+1)){
    Serial.print( "T = " );
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );
  }
    
  delay(1000);
}
